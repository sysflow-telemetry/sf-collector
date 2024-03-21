MODPREFIX=/usr/local/sysflow/modules
DEBUG=0
ASAN=0

rm ./src/libs/.*.o
rm ./src/libs/libsysflow_with_deps.a
rm ./src/libs/libsysflow.a

make -C ./src/libs \
	          SYSFLOW_BUILD_NUMBER=$BUILD_NUMBER \
		           LIBLOCALPREFIX=${MODPREFIX} \
			            FALCOLOCALLIBPREFIX=${MODPREFIX}/lib/falcosecurity \
				             FALCOLOCALINCPREFIX=${MODPREFIX}/include/falcosecurity \
					              AVRLOCALLIBPREFIX=${MODPREFIX}/lib \
						               AVRLOCALINCPREFIX=${MODPREFIX}/include \
							                SFLOCALINCPREFIX=${MODPREFIX}/include/sysflow/c++ \
									         FSLOCALINCPREFIX=${MODPREFIX}/include/filesystem \
										          SCHLOCALPREFIX=${MODPREFIX}/conf \
											           DEBUG=${DEBUG} \
												            ASAN=${ASAN} \
													             install
