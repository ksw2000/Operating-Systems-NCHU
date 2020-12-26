:: This bat will add a container
:: specify container name = %1

@echo off
IF "%~1" == "" GOTO NoName
echo run myubuntu...
docker run -v %cd%/Lab:/Lab -v %cd%/HW:/HW --name %1 -it myubuntu

:NoName
docker run -v %cd%/Lab:/Lab -v %cd%/HW:/HW -it myubuntu
