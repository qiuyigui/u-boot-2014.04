#export PATH=$PATH:/opt/tiny210/toolschain4.7/bin
export CROSS_COMPILE=/opt/tiny210/toolschain/4.5.1/bin/arm-linux-
echo 'make distclean';
make distclean
echo 'make smdkv210_config';
make smdkv210_config
echo 'make ...';
#make spl/u-boot-spl.bin
make all
