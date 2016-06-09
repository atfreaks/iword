/*
iWord -- Fast word seeker.
Mem: (word count)*8 bytes
CPU: O(N)

wikipedia日本語の領域であれば，10MB前後の領域を使用する．
格納できる最大単語数は約1億語．

iWordは共有メモリを使用しているので，ファイルの読み込みは
OSを起動した後に1度のみで良く，iword_load, iword_unloadは
普通のプログラムでは使用不要．
普段はiword_seek，iword_mapを使い，iwordの機能を使う．

*/

#ifndef _IWORD_H
#define _IWORD_H

// iWordの設定
#define IWORD_VERSION       "0.7.1 2010.01.06"
#define IWORD_BSIZE         6
#define IWORD_KEY           0x73faaa01

// iWordの定数
#define IWORD_MODE_HTML     0x1
#define IWORD_MODE_FORBID   0x2
#define IWORD_MODE_ENGLISH  0x4
#define IWORD_FORBID_NUM    5
#define IWORD_FORBIDDEN(a)  (((a)&0xff00)<(IWORD_FORBID_NUM<<8))
#define IWORD_PUBLIC(a)     (((a)&0xff00)>=(IWORD_FORBID_NUM<<8))

#define IWORD_KEY_HIDDEN    0
#define IWORD_KEY_ADULT     1
#define IWORD_KEY_SPAM      2

// 子プログラム用
int iword_seek(char *s);
long long *iword_map(char *s, int size, int mode);
int iword_mask();
void iword_set_limit(int num);
void iword_set_key(int key);
void iword_set_strkey(char *str, int len);

// 親プログラム用
int iword_load(char *filename);
int iword_unload();

// デバッグ用
char *iword_data();
int iword_needed_size();

#endif
