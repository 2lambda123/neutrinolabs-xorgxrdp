# AC_PROG_NASM
# --------------------------
# Check that NASM exists and determine flags
AC_DEFUN([AC_PROG_NASM],[

AC_CHECK_PROGS(NASM, [nasm nasmw yasm])
test -z "$NASM" && AC_MSG_ERROR([no nasm (Netwide Assembler) found])

AC_CHECK_SIZEOF([int *])
ptr_size="$ac_cv_sizeof_int_p"

AC_MSG_CHECKING([for object file format of host system])
objfmt="unknown"
case "$host_os" in
  cygwin* | mingw* | pw32* | interix*)
    case "$ptr_size" in
      8)
        objfmt='Win64-COFF'
        ;;
      4)
        objfmt='Win32-COFF'
        ;;
    esac
  ;;
  msdosdjgpp* | go32*)
    objfmt='COFF'
  ;;
  os2-emx*)			# not tested
    objfmt='MSOMF'		# obj
  ;;
  linux*coff* | linux*oldld*)
    objfmt='COFF'		# ???
  ;;
  linux*aout*)
    objfmt='a.out'
  ;;
  linux*)
    case "$ptr_size" in
      8)
        objfmt='ELF64'
        ;;
      4)
        objfmt='ELF'
        ;;
    esac
  ;;
  kfreebsd* | freebsd* | netbsd* | openbsd*)
    if echo __ELF__ | $CC -E - | grep __ELF__ > /dev/null; then
      objfmt='BSD-a.out'
    else
      case "$ptr_size" in
        8)
          objfmt='ELF64'
          ;;
        4)
          objfmt='ELF'
          ;;
      esac
    fi
  ;;
  solaris* | sunos* | sysv* | sco*)
    case "$ptr_size" in
      8)
        objfmt='ELF64'
        ;;
      4)
        objfmt='ELF'
        ;;
    esac
  ;;
  darwin* | rhapsody* | nextstep* | openstep* | macos*)
    case "$ptr_size" in
      8)
        objfmt='Mach-O64'
        ;;
      4)
        objfmt='Mach-O'
        ;;
    esac
  ;;
esac

AC_MSG_RESULT([$objfmt])
if test "$objfmt" = 'unknown'; then
  objfmt='ELF'
  AC_MSG_WARN([unexpected host system. assumed that the format is $objfmt.])
fi

AC_MSG_CHECKING([for object file format specifier (NAFLAGS) ])
case "$objfmt" in
  MSOMF)      NAFLAGS='-fobj -DOBJ32';;
  Win32-COFF) NAFLAGS='-fwin32 -DWIN32';;
  Win64-COFF) NAFLAGS='-fwin64 -DWIN64 -D__x86_64__';;
  COFF)       NAFLAGS='-fcoff -DCOFF';;
  a.out)      NAFLAGS='-faout -DAOUT';;
  BSD-a.out)  NAFLAGS='-faoutb -DAOUT';;
  ELF)        NAFLAGS='-felf -DELF';;
  ELF64)      NAFLAGS='-felf64 -DELF -D__x86_64__';;
  RDF)        NAFLAGS='-frdf -DRDF';;
  Mach-O)     NAFLAGS='-fmacho -DMACHO';;
  Mach-O64)   NAFLAGS='-fmacho64 -DMACHO -D__x86_64__';;
esac
AC_MSG_RESULT([$NAFLAGS])
AC_SUBST([NAFLAGS])

AC_MSG_CHECKING([whether the assembler ($NASM $NAFLAGS) works])
cat > conftest.asm <<EOF
[%line __oline__ "configure"
        section .text
        global  _nasmfunc, nasmfunc
_nasmfunc:
nasmfunc:
        xor     eax, eax
        ret
]EOF
try_nasm='$NASM $NAFLAGS -o conftest-nasm.o conftest.asm'
if AC_TRY_EVAL(try_nasm) && test -s conftest-nasm.o; then
  AC_MSG_RESULT(yes)
else
  echo "configure: failed program was:" >&AC_FD_CC
  cat conftest.asm >&AC_FD_CC
  rm -rf conftest*
  AC_MSG_RESULT(no)
  AC_MSG_ERROR([installation or configuration problem: assembler cannot create object files.])
fi

AC_MSG_CHECKING([whether the linker accepts assembler output])
nasm_save_LIBS="$LIBS"
LIBS="conftest-nasm.o $LIBS"
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([[
    #ifdef __cplusplus
    extern "C"
    #endif
    void nasmfunc(void);
  ]], [[nasmfunc();]])], [nasm_link_ok=yes], [nasm_link_ok=no])
LIBS="$nasm_save_LIBS"
AC_MSG_RESULT([$nasm_link_ok])

if test "x$nasm_link_ok" = "xno"; then
  AC_MSG_ERROR([configuration problem: maybe object file format mismatch.])
fi

])

