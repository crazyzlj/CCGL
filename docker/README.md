
The <tag> can be `apline`, `apline-with-gdal`, `apline-with-mongodb`, and `apline-with-gdal-mongodb`.
The <ver> can be `latest` or any others, e.g., `v1.0`.
```
cd CCGL
docker buildx create --use
docker buildx build --platform linux/amd64,linux/arm64 --push -t crazyzlj/ccgl:<tag>-<ver> -f docker/alpine/Dockerfile .
```