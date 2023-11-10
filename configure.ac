AC_INIT(configure.in, 1.0)

AC_CANONICAL_HOST


AC_PROG_CXX(cxx CC g++)


AM_INIT_AUTOMAKE (srs,1.00)

AC_PROG_INSTALL

AC_PROG_LIBTOOL

AC_CHECK_HEADER([rpc/rpc.h], [ ],
                [AC_SUBST(RPC_INCLUDE, "-I/usr/include/tirpc")] [AC_SUBST(RPC_LDD, "-ltirpc")])

CXXFLAGS=" -g -O2"


AC_ARG_WITH([srsip],
        [AS_HELP_STRING([--with-srsip=ip : default IP Address of the SRS crate])],
	        [AC_DEFINE_UNQUOTED([SRSIP], ["$withval"])],
		[AC_DEFINE_UNQUOTED([SRSIP], ["10.0.0.2"])])

AC_OUTPUT(Makefile)

