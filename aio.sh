#!/bin/bash
usage(){
	echo "Usage: $0 [-h|-b|-c|-p] -- program to build AB and pack the bin and dependencies" 1>&2
	echo "where:" 1>&2
	echo "  -h  shows this message" 1>&2
	echo "  -b  builds AB and places everything in the bin directory" 1>&2
 	echo "  -c  Cleans out all the binary files" 1>&2
  echo "  -p  Builds a tarball in addition to building everything" 1>&2
	exit 1 ;
}

clean(){
	rm -rf bin
	rm -f bin.tar.gz
	make -C ./src clean
  make -C ./curl-7.51.0 clean
}

build(){
	sudo apt-get install gcc python python-pip -y
	sudo pip install pystatsd
	clean
	set -e
	mkdir bin
	cd ./curl-7.51.0/
	./configure
	make
	cd ../src
	make
	cp ab_httpd ../bin
	cd ../
	mkdir ./bin/libs
	find ./ -name "*.so*" -exec cp {} ./bin/libs \;
	find ./src -name "*.lua" -exec cp {} ./bin \;
  set +e
}

while getopts "bchp" opt;
do
	case $opt in
		h)
			echo "I am here to help you"
      usage
			;;
		p)
			# echo "-f was triggered, Parameter: $OPTARG" >&2
			build
			tar -cvzf bin.tar.gz ./bin
			exit 0
      ;;
    b)
      build
      exit 0
      ;;
    c)
      clean
      exit 0
      ;;
    \?)
			echo "Invalid option: -$OPTARG" 1>&2
			exit 1
			;;
		:)
			echo "Option -$OPTARG requires an argument." 1>&2
			exit 1
			;;
	esac
done
usage
