/* iWord PECL Header */
#ifndef PHP_IWORD_H
#define PHP_IWORD_H

extern zend_module_entry iword_module_entry;
#define phpext_iword_ptr &iword_module_entry

#ifdef PHP_WIN32
#	define PHP_IWORD_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_IWORD_API __attribute__ ((visibility("default")))
#else
#	define PHP_IWORD_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(iword);
PHP_MSHUTDOWN_FUNCTION(iword);
PHP_RINIT_FUNCTION(iword);
PHP_RSHUTDOWN_FUNCTION(iword);
PHP_MINFO_FUNCTION(iword);

PHP_FUNCTION(iword_set);
PHP_FUNCTION(iword_unset);
PHP_FUNCTION(iword_limit);
PHP_FUNCTION(iword_dictionary);
PHP_FUNCTION(iword_map);
PHP_FUNCTION(iword_get_key);
PHP_FUNCTION(iword_get_spam);
PHP_FUNCTION(iword_get_adult);
PHP_FUNCTION(iword_exists);

#ifdef ZTS
#define IWORD_G(v) TSRMG(iword_globals_id, zend_iword_globals *, v)
#else
#define IWORD_G(v) (iword_globals.v)
#endif

// グローバル変数の設定
ZEND_BEGIN_MODULE_GLOBALS(iword)
	// マップ用メモリ
	long long *imap_global;
	// 読み込み文字列とその長さ
	char *str_global; int str_len_global;
ZEND_END_MODULE_GLOBALS(iword)

#endif
