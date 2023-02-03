#!/bin/sh

# please add here your boost installation path
BOOST_INCLUDE_DIR=/cycles_adv/cycles_addon/fastpocket/boost_1_80_0

function prepareDir()
{
    rm -rf $1  
    mkdir -p $1 
    cd $1
}

prepareDir build/$SMTE_SYS

pwd

cmake \
-DBOOST_ROOT=$BOOST_INCLUDE_DIR -DBOOST_INCLUDEDIR=$BOOST_INCLUDE_DIR/boost \
../../src 

gmake
