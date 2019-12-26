#!/usr/bin/env bash


die() {
    >&2 echo $1
    exit 1
}


run_instances() {
    local N=$1
    local EC2_TYPE=$2

    if [[ -z "$EC2_TYPE" ]]; then
        EC2_TYPE="t2.micro"
    fi

    if [[ -z "$N" ]]; then
        die "Specify number of nodes"
    fi

    local INSTANCE_INFO=`aws ec2 run-instances \
        --image-id ami-0d5d9d301c853a04a --security-group-ids $SECURITY_GROUPS --instance-type "$EC2_TYPE" \
        --key-name "$KEY_NAME" --count $N --output json`

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

    for attempt in $(seq 5); do
        if (( attempt != 1 )); then
            sleep 1
        fi

        local INSTANCE_INFO=`aws ec2 describe-instances --instance-ids $INSTANCE_IDS --output json`
        if (( $? != 0 )); then
            continue
        fi

        local INSTANCE_IPS=$(echo "$INSTANCE_INFO" |  jq -r "recurse | .Instances? | select(. != null) | .[] | .PublicIpAddress")
        if (( $? != 0 )); then
            continue
        fi

        for ip in $INSTANCE_IPS; do
            if [[ "$ip" == "null" ]]; then
                continue 2
            fi
        done

        echo $INSTANCE_IPS
        return 0
    done

    die "Failed to retrieve instance info"
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

        res=$(ssh -o "StrictHostKeyChecking no" -i "$PRIVATE_KEY_PATH" ubuntu@"${IP}" "${CMD}")
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
