FROM golang:1.13-alpine as builder
  
RUN apk add --no-cache make gcc musl-dev linux-headers git

WORKDIR /go/go-lachesis


RUN git clone https://github.com/Fantom-foundation/go-lachesis.git . && \
    git checkout scope3

RUN go build \
    -o /tmp/lachesis \
    ./cmd/lachesis



FROM alpine:latest

RUN apk add --no-cache ca-certificates

COPY --from=builder /tmp/lachesis /

EXPOSE 5050 3000

# ENTRYPOINT ["/lachesis", "--nousb", "--port=5050", "--rpc", "--rpcaddr=0.0.0.0", "--rpcport=3000", "--rpccorsdomain=*", "--rpcapi='eth,debug,admin,web3'"]
ENTRYPOINT ["/lachesis"]