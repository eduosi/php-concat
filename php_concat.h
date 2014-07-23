/*
  +-------------------------------------------------------------------------+
  | PHP Version 5                                                        	|
  +-------------------------------------------------------------------------+
  | Copyright (c) 1997-2011 The PHP Group                                	|
  +-------------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      	|
  | that is bundled with this package in the file LICENSE, and is        	|
  | available through the world-wide-web at the following url:           	|
  | http://www.php.net/license/3_01.txt                                  	|
  | If you did not receive a copy of the PHP license and are unable to   	|
  | obtain it through the world-wide-web, please send a note to          	|
  | license@php.net so we can mail you a copy immediately.               	|
  +-------------------------------------------------------------------------+
  | License: http://concat.buession.com.cn/LICENSE                        	|
  | Author: Yong.Teng <webmaster@concat.com>                         		|
  | Copyright @ 2013-2014 Buession.com Inc.									|
  +-------------------------------------------------------------------------+
*/

/* $Id: concat.h 310447 2014-06-18 00:04:23 Yong.Teng $ */

#ifndef PHP_CONCAT_H
#define PHP_CONCAT_H

extern zend_module_entry concat_module_entry;
#define phpext_concat_ptr &concat_module_entry

#ifdef PHP_WIN32
#	ifdef CONCAT_EXPORTS
#		define CONCAT_API __declspec(dllexport)
#	else
#		define CONCAT_API __declspec(dllimport)
#	endif
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define CONCAT_API __attribute__ ((visibility("default")))
#else
#	define CONCAT_API
#endif

#ifdef ZTS
	#include "TSRM.h"
#endif

#ifndef NULL
	#define NULL  ((void *) 0)
#endif

#ifndef TRUE
	#define TRUE 1
#endif
#ifndef FALSE
	#define FALSE 0
#endif

extern zend_class_entry *concat_ce;

#define CONCAT_ME(method, arguments, flags)				ZEND_ME(concat, method, arguments, flags)
#define CONCAT_MALIAS(alias, method, arguments, flags) 	ZEND_MALIAS(concat, alias, method, arguments, flags)
#define CONCAT_STATIC_ME(method, arguments)				ZEND_ME(concat, method, arguments, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
#define CONCAT_STATIC_MALIAS(alias, method, arguments) 	ZEND_MALIAS(concat, alias, method, arguments, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)

#define CONCAT 					"Concat"
#define CONCAT_MAJOR_VERSION	0
#define CONCAT_MINOR_VERSION	0
#define CONCAT_RELEASE_VERSION	2
#define CONCAT_EXTRA_VERSION	"Alpha"
#define CONCAT_VERSION 			ZEND_TOSTR(CONCAT_MAJOR_VERSION)"."ZEND_TOSTR(CONCAT_MINOR_VERSION)"."ZEND_TOSTR(CONCAT_RELEASE_VERSION)
#define CONCAT_BUILD			""
#define CONCAT_API_VERSION 		ZEND_TOSTR(PHP_API_VERSION)
#define CONCAT_REVISION_ID		"$Revision: a64f8304bca2cdac160425047cb96e6640b929b9 $"	// sha1("Major.Minor.Release Extra Build")
#define CONCAT_SUPPORT_URL 		"http://www.buession.com/"
#define CONCAT_SUPPORT_EMAIL 	"webmaster@buession.com"

CONCAT_API int concat_convert_object_to_string_ex(zend_class_entry *ce, zval *object, zend_bool show_error TSRMLS_DC);

CONCAT_API char *concat_object_valueof(zval *data, size_t *result_length TSRMLS_DC);

CONCAT_API zend_bool concat_isBlank_ex(const char *str, uint str_length TSRMLS_DC);

#define CONCAT_APPEND_VERSION(result)	\
	do{	\
		if(CONCAT_G(version_prefix)&&CONCAT_G(version_prefix_length) > 0){	\
			smart_str_appendl(result, "?", 1);	\
			smart_str_appendl(result, CONCAT_G(version_prefix), CONCAT_G(version_prefix_length));	\
			smart_str_appendl(result, "=", 1);	\
			if(CONCAT_G(version)&&CONCAT_G(version_length) > 0){	\
				smart_str_appendl(result, CONCAT_G(version), CONCAT_G(version_length));	\
			}	\
		}	\
	}while(0);

#define CONCAT_FILES(max, files, result){	\
	uint j = 0;	\
	for(; j < max; j++){	\
		char *filename = NULL;	\
		size_t filename_length = 0;	\
		if(j > 0&&CONCAT_G(delimiter)&&CONCAT_G(delimiter_length) > 0){	\
			smart_str_appendl(result, CONCAT_G(delimiter), CONCAT_G(delimiter_length));	\
		}	\
		filename = concat_object_valueof(*(files[j]), &filename_length TSRMLS_CC);	\
		if(filename&&filename_length > 0){	\
			smart_str_appendl(result, filename, filename_length);	\
		}	\
		concat_free(filename);	\
	}	\
	CONCAT_APPEND_VERSION(result);	\
}

#define CONCAT_SETSTRINGL_METHOD(name){	\
	char *value;	\
	uint value_length;	\
	if(zend_parse_parameters(1 TSRMLS_CC, "s", &value, &value_length) == SUCCESS){	\
		concat_alter_ini_entry(name, value, value_length);	\
	}	\
}

#define CONCAT_GETSTRINGL_METHOD(name){	\
	if(return_value_used){	\
		RETURN_STRINGL(CONCAT_G(name), CONCAT_G(name##_length), TRUE);	\
	}	\
}

#define CONCAT_SETLONG_METHOD(name){	\
	long value;	\
	if(zend_parse_parameters(1 TSRMLS_CC, "l", &value) == SUCCESS){	\
		smart_str str = {0};	\
		smart_str_append_long(&str, value);	\
		smart_str_0(&str);	\
		concat_alter_ini_entry(name, str.c, str.len);	\
		smart_str_free(&str);	\
	}	\
}

#define CONCAT_GETLONG_METHOD(name){	\
	RETURN_LONG(CONCAT_G(name));	\
}

#define CONCAT_SETBOOLEAN_METHOD(name){	\
	zend_bool value;	\
	if(zend_parse_parameters(1 TSRMLS_CC, "b", &value) == SUCCESS){	\
		if(value == TRUE){	\
			concat_alter_ini_entry(name, "on", 2);	\
		}else{	\
			concat_alter_ini_entry(name, "off", 3);	\
		}	\
	}	\
}

#define CONCAT_GETBOOLEAN_METHOD(name){	\
	RETURN_BOOL(CONCAT_G(name));	\
}

#define concat_alter_ini_entry(name, value, value_length) \
	zend_alter_ini_entry_ex("concat."#name, sizeof("concat."#name), value, value_length, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, FALSE TSRMLS_CC)

#define concat_free(v){	\
	if(v){	\
		efree(v);	\
		v = NULL;	\
	}	\
}
#define concat_pfree(v, is_persistent){	\
	if(v){	\
		pefree(v, is_persistent);	\
		v = NULL;	\
	}	\
}
#define concat_zval_dtor(z){	\
	if(z){	\
		zval_dtor(z);	\
		z = NULL;	\
	}	\
}
#define concat_zval_free(z){	\
	if(z){	\
		zval_dtor(z);	\
		efree(z);	\
		z = NULL;	\
	}	\
}
#define concat_zval_ptr_dtor(z){	\
	if(z){	\
		zval_ptr_dtor(&z);	\
		z = NULL;	\
	}	\
}
#define concat_zval_ptr_free(z){	\
	if(z){	\
		zval_ptr_dtor(&z);	\
		efree(z);	\
		z = NULL;	\
	}	\
}
#define concat_zval_pptr_dtor(z){	\
	if(z){	\
		zval_ptr_dtor(z);	\
		z = NULL;	\
	}	\
}
#define concat_zval_pptr_free(z){	\
	if(z){	\
		zval_ptr_dtor(z);	\
		efree(z);	\
	}	\
}
#define concat_hash_destroy(ht){	\
	if(ht){	\
		zend_hash_destroy(ht);	\
		ht = NULL;	\
	}	\
}
#define concat_hash_free(ht){	\
	if(ht){	\
		zend_hash_destroy(ht);	\
		efree(ht);	\
		ht = NULL;	\
	}	\
}

ZEND_BEGIN_MODULE_GLOBALS(concat)
	zend_bool enable;
	char *prefix;
	uint prefix_length;
	char *delimiter;
	uint delimiter_length;
	char *version_prefix;
	uint version_prefix_length;
	char *version;
	uint version_length;
	long max_files;
ZEND_END_MODULE_GLOBALS(concat)

ZEND_EXTERN_MODULE_GLOBALS(concat)

#define CONCAT_CLEAN_STRING_G(fields){	\
	concat_free(CONCAT_G(fields));	\
	CONCAT_G(fields##_length) = 0;	\
}

#ifdef ZTS
	#define CONCAT_G(v) TSRMG(concat_globals_id, zend_concat_globals *, v)
#else
	#define CONCAT_G(v) (concat_globals.v)
#endif

PHP_MINIT_FUNCTION(concat);
PHP_RINIT_FUNCTION(concat);
PHP_RSHUTDOWN_FUNCTION(concat);
PHP_MSHUTDOWN_FUNCTION(concat);
PHP_MINFO_FUNCTION(concat);

#endif	/* PHP_CONCAT_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */