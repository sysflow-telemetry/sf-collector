# MinIO
To obtain keys:
```
./run
sudo docker logs minio
```

To use MinIO client: 
Command-line Access: https://docs.minio.io/docs/minio-client-quickstart-guide
```
$ mc config host add minio http://172.17.0.4:9000 <accesskey> <secretkey> 
$ mc ls minio/play
$ mc cp --recursive minio/play /tmp/
$ mc cp --recursive minio/play /tmp/
$ mc cp --recursive /tmp/play/* minio/play/
```


