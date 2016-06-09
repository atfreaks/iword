PHP_ARG_ENABLE(iword, whether to enable iword support,
Make sure that the comment is aligned:
[  --enable-iword           Enable iword support])

if test "$PHP_IWORD" != "no"; then
  PHP_NEW_EXTENSION(iword, iword.c include/iword.c, $ext_shared)
  PHP_SUBST(IWORD_SHARED_LIBADD)
fi
