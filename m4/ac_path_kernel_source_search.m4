dnl check for kernel source
AC_DEFUN([AC_PATH_KERNEL_SOURCE_SEARCH],
[
  kerneldir=missing
  kernelext=ko
  no_kernel=yes

  if test `uname` != "Linux"; then
    kerneldir="not running Linux"
  else
    for dir in /usr/src/kernels/`uname -r` /usr/src/linux-`uname -r` /usr/src/linux/lib/modules/`uname -r`/build ${ac_kerneldir}; do
      if test -d $dir; then
        kerneldir=`dirname $dir/Makefile`/
        no_kernel=no
      fi;
    done
  fi

  if test x${no_kernel} != xyes; then
    if test -f ${kerneldir}/Makefile; then
      if test "${ac_pkss_mktemp}" = "yes"; then
        ac_pkss_makefile=`mktemp /tmp/OLINUXINO.XXXXXX`
      else
        ac_pkss_makefile=/tmp/OLINUXINO.XXXXXX
      fi
      cat ${kerneldir}/Makefile >${ac_pkss_makefile}
      echo "olinuxino_tell_me_what_cc_is:" >>${ac_pkss_makefile}
      echo "	echo \$(CC)" >>${ac_pkss_makefile}

      kernelcc=`make -s -C ${kerneldir} -f ${ac_pkss_makefile} olinuxino_tell_me_what_cc_is`

      echo "olinuxino_tell_me_what_version_is:" >>${ac_pkss_makefile}
      echo "	echo \$(VERSION)" >>${ac_pkss_makefile}
      echo "olinuxino_tell_me_what_patchlevel_is:" >>${ac_pkss_makefile}
      echo "	echo \$(PATCHLEVEL)" >>${ac_pkss_makefile}
      version=`make -s -C ${kerneldir} -f ${ac_pkss_makefile} olinuxino_tell_me_what_version_is`
      patchlevel=`make -s -C ${kerneldir} -f ${ac_pkss_makefile} olinuxino_tell_me_what_patchlevel_is`
      if test ${version} -eq 2; then
        if test ${patchlevel} -lt 5; then
          kernelext=o
        fi
      fi
      rm -f ${ac_pkss_makefile}
    else
      kerneldir="no Makefile found"
      no_kernel=yes
    fi
  fi
  ac_cv_have_kernel="no_kernel=${no_kernel} \
    kerneldir=\"${kerneldir}\" \
    kernelext=\"${kernelext}\" \
    kernelcc=\"${kernelcc}\""
]
)

AC_DEFUN([AC_PATH_KERNEL_SOURCE],
[
  AC_CHECK_PROG(ac_pkss_mktemp,mktemp,yes,no)
  AC_PROVIDE([AC_PATH_KERNEL_SOURCE])
  AC_MSG_CHECKING(for Linux kernel sources)
  AC_ARG_WITH([kerneldir],
              [AS_HELP_STRING([--with-kerneldir=DIR],[kernel sources in DIR @<:@default="/usr/src/linux"@:>@])],
              [ac_kerneldir=${withval}],
              [ac_kerneldir="/usr/src/linux"],
              AC_CACHE_VAL(ac_cv_have_kernel,AC_PATH_KERNEL_SOURCE_SEARCH)
  )

  eval "$ac_cv_have_kernel"

  AC_SUBST(kerneldir)
  AC_SUBST(kernelcc)
  AC_SUBST(kernelext)
  AC_MSG_RESULT(${kerneldir})
]
)
