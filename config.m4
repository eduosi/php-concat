dnl $Id$
dnl config.m4 for extension concat

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(concat, for Concat support,
dnl Make sure that the comment is aligned:
dnl [  --with-concat             enable Concat support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(concat, whether to enable Concat support,
dnl Make sure that the comment is aligned:
dnl [  --enable-concat           enable Concat support])

PHP_ARG_ENABLE(concat, whether to enable concat support,
	[  --enable-concat			enable concat support])

if test "$PHP_CONCAT" != "no"; then
	AC_MSG_CHECKING([PHP version])

	tmp_version=$PHP_VERSION
	if test -z "$tmp_version"; then
		if test -z "$PHP_CONFIG"; then
			AC_MSG_ERROR([php-config not found])
		fi

		php_version=`$PHP_CONFIG --version 2>/dev/null|head -n 1|sed -e 's#\(\d\.\d*\.\d*\)\(.*\)#\1#'`
	else
		php_version=`echo "$tmp_version"|sed -e 's#\(\d\.\d*\.\d*\)\(.*\)#\1#'`
	fi

	if test -z "$php_version"; then
		AC_MSG_ERROR([Failed to detect PHP version, please report])
	fi

	ac_IFS=$IFS
  	IFS="."
  	set $php_version
  	IFS=$ac_IFS
  	concat_php_version=`expr [$]1 \* 1000000 + [$]2 \* 1000 + [$]3`

  	if test "$concat_php_version" -le "5003000"; then
    	AC_MSG_ERROR([You need at least PHP 5.3.0 to be able to use this version of Concat. PHP $php_version found])
  	else
    	AC_MSG_RESULT([$php_version, ok])
  	fi

	PHP_NEW_EXTENSION(concat, [concat.c], $ext_shared)

	AC_DEFINE(HAVE_CONCAT, 1, [Concat Support])
fi