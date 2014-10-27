#!/bin/sh
arg=$1

function checkstatus {
    pass=0
    if [ $1 = $pass ]
    then
        echo "INFO: $2 completed";
    else
        echo "INFO: $2 failed";
        if [ -n "$3" ]
        then
	    echo "INFO: For details check $3 "
	    echo "******************** ERROR ********************";
	    tail $3
	    echo "******************** ERROR ********************";
        fi    
	exit $?
    fi
}
function clean {
    echo "INFO: cleaning build directories"
	rm -rf gmake.out.*;
    make -f GNUmakefile.tnse clean 2>&1 | tee gmake.out.clean ;
    checkstatus $? "gmake.out.clean"
}

function build32bit {
    echo "INFO: Building JToolkit 32 bit libraries"
    echo "INFO: Started the build... "
	make -f GNUmakefile.tnse ARCH_DATA_MODEL=32 2>&1 | tee gmake.out.32bit;
    checkstatus $? "Building 32bit libraries " "gmake.out.32bit"
}

function build64bit {
    echo "INFO: Building JToolkit 64 bit libraries"
    echo "INFO: Started the build... "
	make -f GNUmakefile.tnse ARCH_DATA_MODEL=64  2>&1 | tee gmake.out.64bit;
    checkstatus $? "Building 64bit libraries" "gmake.out.64bit"
}

function package {
    echo "INFO: packaging JToolkit libraries"
    echo "INFO: Started the build... "
	make -f GNUmakefile.tnse package  2>&1 | tee gmake.out.package;
    checkstatus $? "packaging jtoolkit " "gmake.out.package"
}

function usage {
    echo "INFO: Usage "
    echo "Options available are "
    echo "all   : To Compeletly build JToolkit(CM Build)"
    echo "32bit : To build 32bit libraries"
    echo "64bit : To build 64bit libraries"
	echo "clean : To clean JToolkit build directories"
}

case $arg in
          all) echo "Building Complete JToolkit"
			   clean
               build32bit
			   build64bit
			   package;;
		32bit) echo "INFO: Building 32bit libraries"
               build32bit;;
		64bit) echo "INFO: Building 64bit libraries"
               build64bit;;
		clean) echo "cleaning JToolkit build directories"
               clean;;
            *) usage;;
esac

