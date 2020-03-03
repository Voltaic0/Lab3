default:
	cd makeprocs; make
	cd s2inject; make
	cd coinject; make
	cd equation1; make
	cd equation2; make
	cd equation3; make

clean:
	cd makeprocs; make clean
	cd s2inject; make clean
	cd coinject; make clean
	cd equation1; make clean
	cd equation2; make clean
	cd equation3; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 2 8; ee469_fixterminal
