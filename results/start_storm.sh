#!/usr/bin/bash

 go mod download

export GIT_COMMIT=$(git rev-list -1 HEAD) && \
    export GIT_DATE=$(git log -1 --date=short --pretty=format:%ct) && \
    export CGO_ENABLED=1 && \
    go build -ldflags "-s -w -X main.gitCommit=$GIT_COMMIT -X main.gitDate=$GIT_DATE" \
    -o /tmp/tx-storm \
    ./cmd/tx-storm

