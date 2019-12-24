#!/usr/bin/env bash


die() {
    >&2 echo $1
    exit 1
}


run_instances() {
    N=$1

    if [[ -z "$N" ]]; then
        die "Specify number of nodes"
    fi

    local INSTANCE_INFO=`aws ec2 run-instances \
        --image-id ami-0d5d9d301c853a04a --security-group-ids sg-048a755d576006bec --instance-type t2.micro \
        --key-name bombarder --count $N --output json`

    if (( $? != 0 )); then
        die "Cannot run instances"
    fi

    local INSTANCE_IDS=$(echo "$INSTANCE_INFO" | jq -r '.Instances[] | .InstanceId')

    if (( $? != 0 )); then
        echo "$INSTANCE_INFO"
        die "Cannot parse instance info"
    fi

    echo "$INSTANCE_IDS"
}

get_instance_ips() {
    local INSTANCE_IDS="$@"
    local INSTANCE_INFO=`aws ec2 describe-instances --instance-ids $INSTANCE_IDS`

    if (( $? != 0 )); then
        die "Cannot retrieve instance info"
    fi

    local INSTANCE_IPS=$(echo "$INSTANCE_INFO" |  jq -r "recurse | .Instances? | select(. != null) | .[] | .PublicIpAddress")

    if (( $? != 0 )); then
        echo "$INSTANCE_INFO"
        die "Cannot parse instance info (2)"
    fi

    echo "$INSTANCE_IPS"
}

attach_and_exec() {
    local IP=$1
    local CMD=$2

    for attempt in $(seq 2)
    do
        if (( attempt > 5 ));
        then
            echo "  - attempt ${attempt}: " >&2
        fi;

        res=$(ssh -i ~/.ssh/bombarder.pem ubuntu@"${IP}" "${CMD}")
        if [ $? -eq 0 ]
        then
            echo $res
            return 0
        else
            sleep 8
        fi
    done
    echo "Failed to attach to ${IP}" >&2
    return 1
}
