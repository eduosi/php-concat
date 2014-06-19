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
  | License: http://concat.concat.com.cn/LICENSE                        	|
  | Author: Yong.Teng <webmaster@concat.com>                         		|
  | Copyright @ 2013-2014 Concat.com Inc.									|
  +-------------------------------------------------------------------------+
*/

/* $Id: concat.c 310447 2011-06-18 00:04:23 Yong.Teng $ */

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

/* include php file */
#include "Zend/zend.h"
#include "Zend/zend_API.h"
#include "Zend/zend_modules.h"
#include "Zend/zend_exceptions.h"
#include "main/php.h"
#include "main/php_main.h"
#include "main/SAPI.h"
#include "ext/standard/info.h"
#include "ext/standard/php_smart_str_public.h"
#include "ext/standard/php_smart_str.h"
#include "ext/spl/spl_exceptions.h"

/* include concat file */
#include "php_concat.h"

/* PHP versions lower than 5.3.0 are not compatible or are causing tests to FAIL */
#if ((PHP_MAJOR_VERSION < 5)||(PHP_MAJOR_VERSION == 5&&PHP_MINOR_VERSION < 3))
	#error PHP 5.3.0 or newer is required. Please upgrade your php version.
#endif

ZEND_DECLARE_MODULE_GLOBALS(concat)

zend_class_entry *concat_ce;

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(concat_setEnable_arguments, FALSE, FALSE, 1)
	ZEND_ARG_INFO(FALSE, enable)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(concat_setPrefix_arguments, FALSE, FALSE, 1)
	ZEND_ARG_INFO(FALSE, prefix)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(concat_setDelimiter_arguments, FALSE, FALSE, 1)
	ZEND_ARG_INFO(FALSE, delimiter)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(concat_setVersionPrefix_arguments, FALSE, FALSE, 1)
	ZEND_ARG_INFO(FALSE, versionPrefix)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(concat_setVersion_arguments, FALSE, FALSE, 1)
	ZEND_ARG_INFO(FALSE, version)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(concat_setMaxFiles_arguments, FALSE, FALSE, 1)
	ZEND_ARG_INFO(FALSE, maxFiles)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(concat_css_arguments, FALSE, FALSE, 1)
	ZEND_ARG_INFO(FALSE, baseUrl)
	ZEND_ARG_INFO(FALSE, ...)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(concat_javascript_arguments, FALSE, FALSE, 1)
	ZEND_ARG_INFO(FALSE, baseUrl)
	ZEND_ARG_INFO(FALSE, ...)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(concat_js_arguments, FALSE, FALSE, 1)
	ZEND_ARG_INFO(FALSE, baseUrl)
	ZEND_ARG_INFO(FALSE, ...)
ZEND_END_ARG_INFO()
/* }}} */

CONCAT_API int concat_convert_object_to_string_ex(zend_class_entry *ce, zval *object, zend_bool show_error TSRMLS_DC){
#if PHP_VERSION_ID >= 50400
	const zend_object_handlers *handlers = Z_OBJ_HT_P(object);
#else
	zend_object_handlers *handlers = Z_OBJ_HT_P(object);
#endif

	if(ce->__tostring&&handlers->cast_object){
		zval dst;

		if(handlers->cast_object(object, &dst, IS_STRING TSRMLS_CC) == FAILURE){
			if(show_error == TRUE){
				smart_str message = {0};
				zval *exception = NULL;

				smart_str_appendl(&message, "Object of class ", sizeof("Object of class ") - 1);
				smart_str_appendl(&message, ce->name, ce->name_length);
				smart_str_appendl(&message, " could not be converted to string.", sizeof(" could not be converted to string.") - 1);
				smart_str_0(&message);

				exception = zend_throw_error_exception(spl_ce_UnexpectedValueException, message.c, 0, E_CORE_WARNING TSRMLS_CC);
				smart_str_free(&message);
				concat_zval_ptr_dtor(exception);
			}
		}else{
			zval_dtor(object);
			Z_TYPE_P(object) = IS_STRING;
			object->value = dst.value;

			return SUCCESS;
		}
	}else{
		if(handlers->get){
			zval *newobject = handlers->get(object TSRMLS_CC);

			if(Z_TYPE_P(newobject) != IS_OBJECT){
				/* for safety - avoid loop */
				zval_dtor(object);
				*object = *newobject;
				FREE_ZVAL(object);
				convert_to_string(object);

				return SUCCESS;
			}
		}
	}

	return FAILURE;
}

CONCAT_API char *concat_object_valueof(zval *data, size_t *result_length TSRMLS_DC){
	switch(Z_TYPE_P(data)){
		case IS_STRING:
			*result_length = Z_STRLEN_P(data);
			return estrndup(Z_STRVAL_P(data), Z_STRLEN_P(data));
			break;
		case IS_NULL:
			*result_length = 4;
			return estrndup("NULL", 4);
			break;
		case IS_BOOL:
			if(Z_BVAL_P(data) == TRUE){
				*result_length = 4;
				return estrndup("true", 4);
			}else{
				*result_length = 5;
				return estrndup("false", 5);
			}
			break;
		case IS_OBJECT: {
				zend_class_entry *ce = Z_OBJCE_P(data);

				if(concat_convert_object_to_string_ex(ce, data, TRUE TSRMLS_CC) == SUCCESS){
					if(Z_TYPE_P(data) == IS_STRING){
						*result_length = Z_STRLEN_P(data);
						return Z_STRVAL_P(data);
					}
				}

				ce = zend_get_class_entry(data TSRMLS_CC);
				*result_length = ce->name_length;
				return estrndup(ce->name, ce->name_length);
			}
			break;
		case IS_ARRAY: {
				zval *exception = zend_throw_error_exception(spl_ce_UnexpectedValueException, "Array could not be converted to string", 0, E_CORE_WARNING TSRMLS_CC);

				concat_zval_ptr_dtor(exception);
				*result_length = 5;

				return estrndup("Array", 5);
			}
			break;
		default: {
				zval str = *data;

				zval_copy_ctor(&str);
				convert_to_string(&str);

				*result_length = Z_STRLEN(str);

				return Z_STRVAL(str);
			}
			break;
	}

	*result_length = 0;
	return estrndup("", 0);
}

static void concat_destroy_globals(TSRMLS_D){
	CONCAT_CLEAN_STRING_G(prefix);
	CONCAT_CLEAN_STRING_G(delimiter);
	CONCAT_CLEAN_STRING_G(version_prefix);
	CONCAT_CLEAN_STRING_G(version);
	//concat_free(concat_globals->prefix);
	//concat_globals->prefix_length = 0;
	//concat_free(concat_globals->delimiter);
	//concat_globals->delimiter_length = 0;
	//concat_free(concat_globals->version_prefix);
	//concat_globals->version_prefix_length = 0;
	//concat_free(concat_globals->version);
	//concat_globals->version_length = 0;
}

/* {{{ ZEND_INI */
ZEND_INI_BEGIN()
	STD_ZEND_INI_BOOLEAN("concat.enable", "off", ZEND_INI_ALL, OnUpdateBool, enable, zend_concat_globals, concat_globals)
	STD_ZEND_INI_ENTRY("concat.prefix", "??", ZEND_INI_ALL, OnUpdateStringUnempty, prefix, zend_concat_globals, concat_globals)
	STD_ZEND_INI_ENTRY("concat.delimiter", ",", ZEND_INI_ALL, OnUpdateStringUnempty, delimiter, zend_concat_globals, concat_globals)
	STD_ZEND_INI_ENTRY("concat.max_files", "0", ZEND_INI_ALL, OnUpdateLong, max_files, zend_concat_globals, concat_globals)
ZEND_INI_END()
/* }}} */

/* {{{ private void Concat::__construct() */
static ZEND_METHOD(concat, __construct){
}
/* }}} */

/* {{{ public void Concat::setEnable(boolean $enable) */
static ZEND_METHOD(concat, setEnable){
	CONCAT_SETBOOLEAN_METHOD(enable);
}
/* }}} */

/* {{{ public boolean Concat::getEnable() */
static ZEND_METHOD(concat, getEnable){
	CONCAT_GETBOOLEAN_METHOD(enable);
}
/* }}} */

/* {{{ public void Concat::setPrefix(string $prefix) */
static ZEND_METHOD(concat, setPrefix){
	CONCAT_SETSTRINGL_METHOD(prefix);
}
/* }}} */

/* {{{ public string Concat::getPrefix() */
static ZEND_METHOD(concat, getPrefix){
	CONCAT_GETSTRINGL_METHOD(prefix);
}
/* }}} */

/* {{{ public void Concat::setDelimiter(string $delimiter) */
static ZEND_METHOD(concat, setDelimiter){
	CONCAT_SETSTRINGL_METHOD(delimiter);
}
/* }}} */

/* {{{ public string Concat::getDelimiter() */
static ZEND_METHOD(concat, getDelimiter){
	CONCAT_GETSTRINGL_METHOD(delimiter);
}
/* }}} */

/* {{{ public void Concat::setVersionPrefix(string $versionPrefix) */
static ZEND_METHOD(concat, setVersionPrefix){
	char *versionPrefix;
	uint versionPrefix_length;

	if(zend_parse_parameters(1 TSRMLS_CC, "s", &versionPrefix, &versionPrefix_length) == SUCCESS){
		CONCAT_CLEAN_STRING_G(version_prefix);

		CONCAT_G(version_prefix) = estrndup(versionPrefix, versionPrefix_length);
		CONCAT_G(version_prefix_length) = versionPrefix_length;
	}
}
/* }}} */

/* {{{ public string Concat::getVersionPrefix() */
static ZEND_METHOD(concat, getVersionPrefix){
	CONCAT_GETSTRINGL_METHOD(version_prefix);
}
/* }}} */

/* {{{ public void Concat::setVersion(string $version) */
static ZEND_METHOD(concat, setVersion){
	char *version;
	uint version_length;

	if(zend_parse_parameters(1 TSRMLS_CC, "s", &version, &version_length) == SUCCESS){
		CONCAT_CLEAN_STRING_G(version);

		CONCAT_G(version) = estrndup(version, version_length);
		CONCAT_G(version_length) = version_length;
	}
}
/* }}} */

/* {{{ public string Concat::getVersion() */
static ZEND_METHOD(concat, getVersion){
	CONCAT_GETSTRINGL_METHOD(version);
}
/* }}} */

/* {{{ public void Concat::setMaxFiles(integer $maxFiles) */
static ZEND_METHOD(concat, setMaxFiles){
	CONCAT_SETLONG_METHOD(max_files);
}
/* }}} */

/* {{{ public integer Concat::getMaxFiles() */
static ZEND_METHOD(concat, getMaxFiles){
	CONCAT_GETLONG_METHOD(max_files);
}
/* }}} */

/* {{{ public string Concat::css(string $baseUrl, mixed ...) */
static ZEND_METHOD(concat, css){
	zval ***arguments = NULL;
	uint arguments_count = 0;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "+", &arguments, &arguments_count) == SUCCESS){
		uint i = 0;
		int size = arguments_count - 1;
		char *baseUrl;
		size_t baseUrl_length;
		uint end = CONCAT_G(enable) == FALSE||CONCAT_G(max_files) == 0 ? size : MIN(CONCAT_G(max_files), size);
		zval ***args = arguments + 1;
		smart_str str = {0};

		baseUrl = concat_object_valueof(*(arguments[0]), &baseUrl_length TSRMLS_CC);

		if(CONCAT_G(enable) == TRUE){
			smart_str_appendl(&str, "<link href=\"", sizeof("<link href=\"") - 1);

			if(baseUrl&&baseUrl_length > 0){
				smart_str_appendl(&str, baseUrl, baseUrl_length);
			}

			if(CONCAT_G(prefix)&&CONCAT_G(prefix_length) > 0){
				smart_str_appendl(&str, CONCAT_G(prefix), CONCAT_G(prefix_length));
			}

			CONCAT_FILES(end, args, &str);

			smart_str_appendl(&str, "\" rel=\"stylesheet\" type=\"text/css\" />", sizeof("\" rel=\"stylesheet\" type=\"text/css\" />") - 1);
		}else{
			for(; i < end; i++){
				char *filename = NULL;
				size_t filename_length = 0;

				smart_str_appendl(&str, "<link href=\"", sizeof("<link href=\"") - 1);

				if(baseUrl&&baseUrl_length > 0){
					smart_str_appendl(&str, baseUrl, baseUrl_length);
				}

				filename = concat_object_valueof(*(args[i]), &filename_length TSRMLS_CC);
				if(filename&&filename_length > 0){
					smart_str_appendl(&str, filename, filename_length);
				}

				CONCAT_APPEND_VERSION(&str);

				smart_str_appendl(&str, "\" rel=\"stylesheet\" type=\"text/css\" />", sizeof("\" rel=\"stylesheet\" type=\"text/css\" />") - 1);
			}
		}

		smart_str_0(&str);
		concat_free(arguments);

		RETURN_STRINGL(str.c, str.len, FALSE);
	}
}
/* }}} */

/* {{{ public string Concat::javascript(string $baseUrl, mixed ...) */
static ZEND_METHOD(concat, javascript){
	zval ***arguments = NULL;
	uint arguments_count = 0;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "+", &arguments, &arguments_count) == SUCCESS){
		uint i = 0;
		int size = arguments_count - 1;
		char *baseUrl;
		size_t baseUrl_length;
		uint end = CONCAT_G(enable) == FALSE||CONCAT_G(max_files) == 0 ? size : MIN(CONCAT_G(max_files), size);
		zval ***args = arguments + 1;
		smart_str str = {0};

		baseUrl = concat_object_valueof(*(arguments[0]), &baseUrl_length TSRMLS_CC);

		if(CONCAT_G(enable) == TRUE){
			smart_str_appendl(&str, "<script src=\"", sizeof("<script src=\"") - 1);

			if(baseUrl&&baseUrl_length > 0){
				smart_str_appendl(&str, baseUrl, baseUrl_length);
			}

			if(CONCAT_G(prefix)&&CONCAT_G(prefix_length) > 0){
				smart_str_appendl(&str, CONCAT_G(prefix), CONCAT_G(prefix_length));
			}

			CONCAT_FILES(end, args, &str);

			smart_str_appendl(&str, "\" type=\"text/javascript\"></script>", sizeof("\" type=\"text/javascript\"></script>") - 1);
		}else{
			for(; i < end; i++){
				char *filename = NULL;
				size_t filename_length = 0;

				smart_str_appendl(&str, "<script src=\"", sizeof("<script src=\"") - 1);

				if(baseUrl&&baseUrl_length > 0){
					smart_str_appendl(&str, baseUrl, baseUrl_length);
				}

				filename = concat_object_valueof(*(args[i]), &filename_length TSRMLS_CC);
				if(filename&&filename_length > 0){
					smart_str_appendl(&str, filename, filename_length);
				}

				CONCAT_APPEND_VERSION(&str);

				smart_str_appendl(&str, "\" type=\"text/javascript\"></script>", sizeof("\" type=\"text/javascript\"></script>") - 1);
			}
		}

		smart_str_0(&str);
		concat_free(arguments);

		RETURN_STRINGL(str.c, str.len, FALSE);
	}
}
/* }}} */

/* {{{ private void Concat::__destruct() */
static ZEND_METHOD(concat, __destruct){
}
/* }}} */

static zend_function_entry concat_methods[] = {
	CONCAT_ME(__construct, NULL, ZEND_ACC_PRIVATE|ZEND_ACC_CTOR)
	CONCAT_STATIC_ME(setEnable, concat_setEnable_arguments)
	CONCAT_STATIC_ME(getEnable, NULL)
	CONCAT_STATIC_ME(setPrefix, concat_setPrefix_arguments)
	CONCAT_STATIC_ME(getPrefix, NULL)
	CONCAT_STATIC_ME(setDelimiter, concat_setDelimiter_arguments)
	CONCAT_STATIC_ME(getDelimiter, NULL)
	CONCAT_STATIC_ME(setVersionPrefix, concat_setVersionPrefix_arguments)
	CONCAT_STATIC_ME(getVersionPrefix, NULL)
	CONCAT_STATIC_ME(setVersion, concat_setVersion_arguments)
	CONCAT_STATIC_ME(getVersion, NULL)
	CONCAT_STATIC_ME(setMaxFiles, concat_setMaxFiles_arguments)
	CONCAT_STATIC_ME(getMaxFiles, NULL)
	CONCAT_STATIC_ME(css, concat_css_arguments)
	CONCAT_STATIC_ME(javascript, concat_javascript_arguments)
	CONCAT_STATIC_MALIAS(js, javascript, concat_js_arguments)
	CONCAT_ME(__destruct, NULL, ZEND_ACC_PRIVATE|ZEND_ACC_DTOR)
	ZEND_FE_END
};

/* {{{ concat_functions */
const zend_function_entry concat_functions[] = {
	ZEND_FE_END
};
/* }}} */

/** {{{ ZEND_MINIT_FUNCTION */
ZEND_MINIT_FUNCTION(concat){
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "Concat", concat_methods);
	concat_ce = zend_register_internal_class(&ce TSRMLS_CC);
	concat_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;

	zend_declare_class_constant_stringl(concat_ce, "NAME", 4, ZEND_STRL(CONCAT) TSRMLS_CC);
	zend_declare_class_constant_stringl(concat_ce, "VERSION", 7, ZEND_STRL(CONCAT_VERSION) TSRMLS_CC);
	zend_declare_class_constant_long(concat_ce, "MAJOR_VERSION", 13, CONCAT_MAJOR_VERSION TSRMLS_CC);
	zend_declare_class_constant_long(concat_ce, "MINOR_VERSION", 13, CONCAT_MINOR_VERSION TSRMLS_CC);
	zend_declare_class_constant_long(concat_ce, "RELEASE_VERSION", 15, CONCAT_RELEASE_VERSION TSRMLS_CC);
	zend_declare_class_constant_stringl(concat_ce, "EXTRA_VERSION", 13, ZEND_STRL(CONCAT_EXTRA_VERSION) TSRMLS_CC);
	zend_declare_class_constant_stringl(concat_ce, "BUILD", 5, ZEND_STRL(CONCAT_BUILD) TSRMLS_CC);
	zend_declare_class_constant_stringl(concat_ce, "API_VERSION", 11, ZEND_STRL(CONCAT_API_VERSION) TSRMLS_CC);
	zend_declare_class_constant_stringl(concat_ce, "REVISION_ID", 11, ZEND_STRL(CONCAT_REVISION_ID) TSRMLS_CC);

	REGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/** {{{ ZEND_RINIT_FUNCTION */
ZEND_RINIT_FUNCTION(concat){
	CONCAT_G(version_prefix) = NULL;
	CONCAT_G(version_prefix_length) = 0;
	CONCAT_G(version) = NULL;
	CONCAT_G(version_length) = 0;

	return SUCCESS;
}
/* }}} */

/** {{{ ZEND_RSHUTDOWN_FUNCTION */
ZEND_RSHUTDOWN_FUNCTION(concat){
	CONCAT_CLEAN_STRING_G(version_prefix);
	CONCAT_CLEAN_STRING_G(version);

	return SUCCESS;
}
/* }}} */

/** {{{ ZEND_MSHUTDOWN_FUNCTION */
ZEND_MSHUTDOWN_FUNCTION(concat){
	concat_destroy_globals(TSRMLS_C);

	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/** {{{ ZEND_MINFO_FUNCTION */
ZEND_MINFO_FUNCTION(concat){
	php_info_print_table_start();
	php_info_print_table_header(2, "concat support", "enabled");
	php_info_print_table_row(2, "Name", CONCAT);
	php_info_print_table_row(2, "Version", CONCAT_VERSION" "CONCAT_EXTRA_VERSION);
	if(CONCAT_BUILD[0] != '\0'){
		php_info_print_table_row(2, "Build", CONCAT_BUILD);
	}
	php_info_print_table_row(2, "API Version", CONCAT_API_VERSION);
	php_info_print_table_row(2, "Revision Id", CONCAT_REVISION_ID);
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/** {{{ ZEND_GINIT_FUNCTION */
ZEND_GINIT_FUNCTION(concat){
	concat_globals->enable = FALSE;
	concat_globals->prefix = estrndup("??", 2);
	concat_globals->prefix_length = 2;
	concat_globals->delimiter = estrndup(",", 1);
	concat_globals->delimiter_length = 1;
	concat_globals->version_prefix = NULL;
	concat_globals->version_prefix_length = 0;
	concat_globals->version = NULL;
	concat_globals->version_length = 0;
	concat_globals->max_files = 0;
}
/* }}} */

/** {{{ ZEND_GSHUTDOWN_FUNCTION */
ZEND_GSHUTDOWN_FUNCTION(concat){
	concat_free(concat_globals->prefix);
	concat_globals->prefix_length = 0;
	concat_free(concat_globals->delimiter);
	concat_globals->delimiter_length = 0;
	concat_free(concat_globals->version_prefix);
	concat_globals->version_prefix_length = 0;
	concat_free(concat_globals->version);
	concat_globals->version_length = 0;
}
/* }}} */

/* {{{ module depends */
zend_module_dep concat_deps[] = {
	{NULL, NULL, NULL}
};
/* }}} */

#ifdef COMPILE_DL_CONCAT
	ZEND_GET_MODULE(concat)
#endif

/* {{{ concat_module_entry */
zend_module_entry concat_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	concat_deps,
	CONCAT,
	concat_functions,
	ZEND_MODULE_STARTUP_N(concat),
	ZEND_MODULE_SHUTDOWN_N(concat),
	ZEND_MODULE_ACTIVATE_N(concat),
	ZEND_MODULE_DEACTIVATE_N(concat),
	ZEND_MINFO(concat),
	CONCAT_VERSION,
	ZEND_MODULE_GLOBALS(concat),
	ZEND_GINIT(concat),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */