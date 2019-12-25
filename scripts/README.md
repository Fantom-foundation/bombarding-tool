Bombarding Deployment
======================

### Usage

Configure AWS CLI first:

```sh
aws configure
# assuming there is a key-pair bombarder.pem bombarder.pub in ~/.ssh/
aws ec2 import-key-pair --key-name bombarder --public-key-material file://~/.ssh/bombarder.pub
```

Start bombarding:

```sh
./bombard.sh N # N - number of nodes
```

Expect the following lines in the end of output (instance ids will be different):

```
Bombarding is successfully set up
To finish bombarding call
bombard-fetch-and-stop.sh i-04a0dc2d116f32723 i-04b6853a824c4a438 i-05f923d7299a66db4
```

So to stop bombarding and fetch logs run:

```sh
./bombard-fetch-and-stop.sh i-04a0dc2d116f32723 i-04b6853a824c4a438 i-05f923d7299a66db4
```

The logs can be examined with log analyzer.

