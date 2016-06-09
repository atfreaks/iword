#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "iword.h"
#include "include/iword.h"

#define RETURN_STR_G {if(str_g){char *mystr=str_g;int mylen=\
	str_len_g;RETURN_STRINGL(mystr,mylen,1);}RETURN_FALSE;}

// iword_set — iwordで処理する文字列の設定
void iword_set(char *arg, int arg_len, long mode) {
	// グローバル変数の初期化
	iword_initialize();
	// 文字列をコピーする
	str_g = (char *)malloc(arg_len);
	memcpy(str_g, arg, str_len_g = arg_len);
	// 単語マップを取得
	imap_g = iword_map(str_g, str_len_g, mode);
}

// ZValとして与える
void iword_set_zval(zval *arg, long mode)
 { iword_set(Z_STRVAL(*arg), Z_STRLEN(*arg), mode | IWORD_MODE_FORBID); }

PHP_FUNCTION(iword_set) {
	long mode = IWORD_MODE_HTML | IWORD_MODE_FORBID;
	char *arg; int arg_len;
	
	// PHPから引数を受け取る
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	 "s|l", &arg, &arg_len, &mode) == FAILURE) return;
	// 文字列をコピーする
	iword_set(arg, arg_len, mode);
}

// iword_limit ― iwordで処理する限界数の設定
PHP_FUNCTION(iword_limit)
{
	long limit;
	
	// PHPから引数を受け取る
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	 "l", &limit) == FAILURE) return;
	// 限界数の設定
	iword_set_limit(limit);
}

// iword_dictionary ― iwordで処理する辞書の設定
PHP_FUNCTION(iword_dictionary)
{
	char *str; int len;
	
	// PHPから引数を受け取る
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	 "s", &str, &len) == FAILURE) return;
	// 限界数の設定
	iword_set_strkey(str, len);
}

// iword_unset — iwordで処理する文字列の解放
PHP_FUNCTION(iword_unset)
 { iword_initialize(); }

// iword_autolink — HTMLとしてオートリンクする
PHP_FUNCTION(iword_autolink)
{
	char *opt = (char *)"<a href=\"http://atpedia.jp/word/%e\">%s</a>";
	int i, j, k, t, opt_len, len, nlen;
	long mode = IWORD_MODE_HTML;
	char *strg, *p, *esc, *escp, *txt, *txtp, c, *optp;
	zval *src = NULL;
	
	// オプションの長さを格納する(パラメータ省略時用)
	opt_len = strlen(opt);
	// PHPから引数を受け取る
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	 "z|sl", &src, &opt, &opt_len, &mode) == FAILURE) return;
	// 引数が渡されていれば
	if (src != NULL) {
		// 引数が文字列でなければ
		if (Z_TYPE_P(src) == IS_STRING) iword_set_zval(src, mode);
		// 値がTrueであれば中断
		else if (Z_BVAL_P(src)) RETURN_FALSE;
	}
	// 単語マップが存在しない場合元の文字列を返す
	if (imap_g == NULL) RETURN_STR_G;
	// 変換後の文字列の長さを推測
	for (i = 0, nlen = str_len_g; imap_g[i]; i++)
	 if ((mode & IWORD_MODE_FORBID) || IWORD_PUBLIC(imap_g[i]))
	  nlen += opt_len + (imap_g[i] & 0xff) * 3;
	// 置換後の文字列を格納する領域を確保
	strg = p = (char *)emalloc(nlen);
	// 処理用のメモリ領域を確保(※スタックにとらないためにemallocしている)
	esc = (char *)emalloc(900); txt = (char *)emalloc(300);
	// 文字列をコピー(iが元の文字列の読み込み位置，jがマップの読み込み位置)
	for (i = j = 0; i < str_len_g && imap_g[j]; j++) {
		// 禁止項目であれば次へ進める
		if (!((mode & IWORD_MODE_FORBID) || IWORD_PUBLIC(imap_g[j]))) continue;
		// 単語が出現するまでコピー
		// for (; i < (ret[j] >> 8); i++) *(p++) = str[i];
		if (0 < (k = (imap_g[j] >> 16) - i))
		 memcpy(p, str_g + i, k), p += k, i += k;
		// エスケープ後の文字列と生の文字列の生成
		escp = esc; txtp = txt;
		// 単語部分の処理
		for (k = imap_g[j] & 0xff; k; k--, i++) {
			c = str_g[i];
			// エスケープ不要文字
			if (isalnum(c) || c == '-' || c == '.' || c == '_') *(escp++) = c;
			// スペースは"+"に変更
			else if (c == ' ') *(escp++) = '+';
			// 強制エスケープ
			else sprintf(escp, "%%%02x", c & 0xff), escp += 3;
			// 生の文字列はそのままコピー
			*(txtp++) = c;
		}
		// 終端位置にNULLを入力
		*escp = *txtp = 0;
		// オプション処理を受ける(sprintfは少々危険なので独自処理)
		//p += sprintf(p, opt, esc, txt);
		for (t = 0; t < opt_len; t++)
		 // 一般文字列に対してはそのままコピー
		 if (opt[t] != '%') *(p++) = opt[t];
		 // %で始まる2文字に対する処理
		 else if (++t == opt_len) *(p++) = '%';
		 else {
			// %の連続は1文字の%にする
			if (opt[t] == '%') *(p++) = '%';
			// %eに対してはエスケープされた文字にする
			else if (opt[t] == 'e')
			 memcpy(p, esc, k = escp - esc), p += k, escp = esc;
			// %sに対しては元の単語にする
			else if (opt[t] == 's')
			 memcpy(p, txt, k = txtp - txt), p += k, txtp = txt;
			// %kに対してはキーをいれる
			else if (opt[t] == 'k') {
				k = (imap_g[j] & 0xff00) >> 8;
				if (10 < k) *(p++) = '1', k -= 10;
				*(p++) = '0' + k;
			} else if (opt[t] == 'K') {
				k = (imap_g[j] & 0xff00) >> 8;
				*(p++) = (10 < k ? 'a' - 10 : '0') + k;
			}
			// その他に対してはそのまま出力する
			else *(p++) = '%', *(p++) = opt[t];
		}
	}
	// 最後の単語以降の処理
	// for (; i < arg_len; i++) *(p++) = arg[i];
	memcpy(p, str_g + i, k = str_len_g - i); p += k;
	// 後始末
	efree(esc); efree(txt);
	// 文字列を返す
	RETURN_STRINGL(strg, p - strg, 0);
}

// iword_autolink2 — 制御コードで単語をマークする
PHP_FUNCTION(iword_autolink2)
{
	char *strg, *p;
	int i, j, k, nlen, len;
	
	// 単語マップが存在しない場合
	if (imap_g == NULL) {
		// 文字列が与えられていればそのまま返す
		if (str_g) {
			char *mystr = str_g; int mylen = str_len_g;
			RETURN_STRINGL(mystr, mylen, 1);
		}
		// さもなくばFALSEを返す
		RETURN_FALSE;
	}
	// 変換後の長さを計算(元の長さ+2*単語数)
	for (nlen = str_len_g, i = 0; imap_g[i]; i++) nlen += 2;
	// 置換後の文字列を格納する領域を確保
	strg = p = (char *)emalloc(nlen);
	// 文字列をコピー(iが元の文字列の読み込み位置，jがマップの読み込み位置)
	for (i = j = 0; i < str_len_g && imap_g[j]; j++) {
		// 単語が出現するまでループ
		for (; i < (imap_g[j] >> 16); i++) *(p++) = str_g[i];
		// 単語の開始として"\x01"をつける
		*(p++) = 1;
		// 単語の内部をコピー
		for (k = imap_g[j] & 0xff; k; k--, i++) *(p++) = str_g[i];
		// 単語の終了として"\x02"をつける
		*(p++) = 2;
	}
	// 最終単語以降の文字列をコピー
	for (; i < str_len_g; i++) *(p++) = str_g[i];
	// 変換後の文字列を返す(emalloc済みなのでduplicate=0)
	RETURN_STRINGL(strg, p - strg, 0);
}

// iword_map — 全単語を抽出する
PHP_FUNCTION(iword_map)
{
	int i; zval *src = NULL; long mode = IWORD_MODE_HTML;
	
	// PHPから引数を受け取る
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	 "|zl", &src, &mode) == FAILURE) return;
	// 引数が渡された場合
	if (src != NULL) {
		// 引数が文字列でなければ
		if (Z_TYPE_P(src) == IS_STRING) iword_set_zval(src, mode);
		// 値がTrueであれば中断
		else if (Z_BVAL_P(src)) RETURN_FALSE;
	}
	// 単語マップの取得に失敗した時は元のFalseを返す
	if (imap_g == NULL) RETURN_FALSE;
	// 返すための配列を用意(return_valueはPHPが宣言済み)
	array_init(return_value);
	// return_value配列に順に格納していく
	for (i = 0; imap_g[i]; i++)
	 if ((mode & IWORD_MODE_FORBID) || IWORD_PUBLIC(imap_g[i]))
	  add_index_long(return_value, imap_g[i] >> 16, imap_g[i] & 0xff);
}

void iword_get_key(zval *return_value, zval *src, int key, long mode) {
	int i;
	
	// 引数が渡された場合
	if (src != NULL) {
		// 引数が文字列でなければ
		if (Z_TYPE_P(src) == IS_STRING) iword_set_zval(src, mode);
		// 値がTrueであれば中断
		else if (Z_BVAL_P(src)) RETURN_FALSE;
	}
	// 単語マップの取得に失敗した時はFalseを返す
	if (imap_g == NULL) RETURN_FALSE;
	// 返すための配列を用意(return_valueはPHPが宣言済み)
	array_init(return_value);
	// return_value配列に順に格納していく
	for (i = 0, key <<= 8; imap_g[i]; i++)
	 if ((imap_g[i] & 0xff00) == key) 
	  add_index_long(return_value, imap_g[i] >> 16, imap_g[i] & 0xff);
}

// iword_get_key — 特定グループの単語を抽出する
PHP_FUNCTION(iword_get_key)
{
	long key; zval *src = NULL; long mode = IWORD_MODE_HTML;
	
	// PHPから引数を受け取る
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	 "l|zl", &key, &src, &mode) == FAILURE) return;
	// スパム語を抜き取る
	iword_get_key(return_value, src, key, mode);
}

// iword_get_spam — SPAM単語を抽出する
PHP_FUNCTION(iword_get_spam)
{
	zval *src = NULL; long mode = IWORD_MODE_HTML;
	
	// PHPから引数を受け取る
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	 "|zl", &src, &mode) == FAILURE) return;
	// スパム語を抜き取る
	iword_get_key(return_value, src, IWORD_KEY_SPAM, mode);
}

// iword_get_adult — アダルト単語を抽出する
PHP_FUNCTION(iword_get_adult)
{
	zval *src = NULL; long mode = IWORD_MODE_HTML;
	
	// PHPから引数を受け取る
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	 "|zl", &src, &mode) == FAILURE) return;
	// スパム語を抜き取る
	iword_get_key(return_value, src, IWORD_KEY_ADULT, mode);
}

PHP_FUNCTION(iword_exists)
{
	long key = 9;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	 "|l", &key) == FAILURE) return;
	if (iword_mask() & (1 << key)) RETURN_TRUE;
	RETURN_FALSE;
}

