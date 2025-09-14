pushd %~dp0
protoc.exe -I=./ --cpp_out=./ ./SharedEnum.proto
protoc.exe -I=./ --cpp_out=./ ./SharedStruct.proto
protoc.exe -I=./ --cpp_out=./ ./Protocol.proto

PAUSE