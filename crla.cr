require "time"
require "option_parser"

struct BlockEvent
    def initialize (@unix_time_ms : Int64, @ntxns : Int64)
    end
end

def process_generic (match, result, assumed_year)
    ntxns = match["ntxns"].to_i64
    return if ntxns == 0

    time = Time.local(
        year:       assumed_year,
        month:      match["month"].to_i,
        day:        match["dd"].to_i,
        hour:       match["hh"].to_i,
        minute:     match["mm"].to_i,
        second:     match["ss"].to_i,
        nanosecond: match["ms"].to_i * 1_000_000)

    hash = match["hash"]

    result[hash] = BlockEvent.new(
        unix_time_ms: time.to_unix_ms,
        ntxns: ntxns)
end

class RawLogParser
    private NEW_BLOCK_RE = /^INFO *\[(?<month>\d+)-(?<dd>\d+)\|(?<hh>\d+):(?<mm>\d+):(?<ss>\d+)\.(?<ms>\d+)\] New block( .*)? (atropos|hash)=(?<hash>\S+)( .*)? txs=(?<ntxns>\d+)/

    def initialize (@assumed_year : Int32)
    end

    def process (file, result)
        file.each_line do |line|
            match = NEW_BLOCK_RE.match(line)
            next unless match
            process_generic(match, result, assumed_year: @assumed_year)
        end
    end
end

class JsonLogParser
    private NEW_BLOCK_RE = /^\{"log":"INFO *\[(?<month>\d+)-(?<dd>\d+)\|(?<hh>\d+):(?<mm>\d+):(?<ss>\d+)\.(?<ms>\d+)\] New block( .*)? (atropos|hash)=(?<hash>\S+)( .*)? txs=(?<ntxns>\d+)/

    def initialize (@assumed_year : Int32)
    end

    def process (file, result)
        file.each_line do |line|
            match = NEW_BLOCK_RE.match(line)
            next unless match
            process_generic(match, result, assumed_year: @assumed_year)
        end
    end
end

def make_parser_for (path, *args, **kwargs)
    File.open(path, "r") do |file|
        if file.read_char == '{'
            return JsonLogParser.new(*args, **kwargs)
        else
            return RawLogParser.new(*args, **kwargs)
        end
    end
    raise "unreachable"
end

def calc_tps (block_evs, ratio)
    n = (block_evs.size * ratio).to_i

    result = 0.0
    ntxns = 0_i64
    (0...block_evs.size).each do |i|
        ntxns += block_evs[i].@ntxns
        next if i < n
        ntxns -= block_evs[i - n].@ntxns
        time_delta_ms = block_evs[i].@unix_time_ms - block_evs[i - n + 1].@unix_time_ms
        next if time_delta_ms <= 0
        tps = ntxns * 1000.0 / time_delta_ms
        result = Math.max(result, tps)
    end
    result
end

def main
    assumed_year = Time.local.year
    ratios = [0.3, 0.4, 0.5, 0.6, 0.7]

    opt_parser = OptionParser.new
    opt_parser.banner = "Usage: crla [options] LOGFILE [LOGFILE ...]"
    opt_parser.on("-y YEAR", "Set year") { |s| assumed_year = s.to_i }
    opt_parser.on("-r RATIO[,RATIO...]", "Set ratio(s)") { |s| ratios = s.split(',').map &.to_i }
    opt_parser.parse
    if ARGV.empty?
        STDERR.puts opt_parser
        exit 2
    end

    result = {} of String => BlockEvent

    ARGV.each do |path|
        parser = make_parser_for(path, assumed_year: assumed_year)
        File.open(path, "r") do |file|
            parser.process(file: file, result: result)
        end
    end

    block_evs = result.values
    block_evs.sort_by! &.@unix_time_ms

    ratios.each do |ratio|
        tps = calc_tps(block_evs, ratio: ratio)
        puts "ratio #{ratio} => TPS #{tps}"
    end
end

main
