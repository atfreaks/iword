<?php
// モジュール名の設定
$module = 'iword';

// CLIであれば"\n"，MODであれば"<br />\n"
$br = (php_sapi_name() == "cli")? "\n":"<br />\n";
// 未ロードであればiWordの動的ロードを行う
if(!extension_loaded('iword')) dl('iword.' . PHP_SHLIB_SUFFIX);
// ロードされていなければ
if (!extension_loaded($module))
 die("{$module}はPHPに組み込まれていません$br");

// 関数のリストを出力
echo "{$module}では以下の関数が有効です:$br";
foreach(get_extension_funcs($module) as $f) echo "$f$br";
echo $br;

// 以下テスト
if (function_exists('iword_map')) {
	test_start();
	iword_set($str = 'あいうえ初音ミクあああ');
	echo "処理する文字: $str$br$br";
	$map = iword_map(false);
	echo "結果: ".count($map)."件ヒットしました$br";
	if (is_array($map)) foreach ($map as $start => $len)
	 echo "$start: ".substr($str, $start, $len).$br;
}

if (function_exists('iword_autolink')) {
	test_start();
	iword_set($str = 'iWordははてなダイアリーの単語抽出のような機能を高速に実装したプログラムです．@freaks向けのプログラムであり，中核となるプログラムは公開していませんので，必要な方は直接連絡を取って下さい．現在の速度は100万単語の辞書に対して390KByte/sec(32-bit php, 2.53GHz Core 2 Duo, DDR3 1067MHz, iWord 0.5.0 環境下)程度の速度です．呼び出し処理は0.05ms以下でオーバーヘッドはほぼないのが特徴です．');
	echo "処理する文字: $str$br$br";
	echo iword_autolink(false, '(%e)[%s]', 1) . $br;
	echo iword_autolink(false) . $br;
	// echo iword_autolink($str) . $br;
	var_dump(iword_map(false));
	echo $br;
	echo $br;
}

if (function_exists('iword_autolink')) {
	test_start();
	iword_set($str = 'core iWord is a program for extracting words from html like Hatena diary. Being made for @freaks, the core of the program is not opened, so please contact directly if you need it. encore wordspace ecoree core', 5);
	echo "処理する文字: $str$br$br";
	echo iword_autolink(false, '(%e)[%s]', 1) . $br;
	echo iword_autolink(false) . $br;
	// echo iword_autolink($str) . $br;
	var_dump(iword_map(false));
	echo $br;
	echo $br;
}

/*
if (function_exists('iword_autolink2')) {
	test_start();
	iword_set($str = 'あいうえ初音ミクあああ単語');
	echo "処理する文字: $str$br$br";
	$data = iword_autolink2();
	echo str_replace(array("\x01", "\x02"), array("[", "]"), $data);
	echo $br;
}
*/

if (function_exists('iword_get_spam')) {
	test_start();
	iword_set('あああアダルトああAdultすすKeywordAすSPamSpamスパム');
	print_r(iword_map());
	print_r(iword_get_spam());
}

if (function_exists('iword_exists')) {
	test_start();
	for ($i = 0; $i < 15; $i++) {
		echo "Key $i " . (iword_exists($i) ? 'exists' : 'does not exists') . ".$br";
	}
}

/*
if (function_exists('iword_set')) {
	test_start();
	$tim = time();
	for ($t = 0; $t < 5; $t++) {
		echo "$t th case: " . $br;
		$str = "";
		for ($i = 0; $i < 30000; $i++) {
			$str .= sha1("$tim@$t@$i");
		}
		$str = str_replace('<', '>', $str);
		echo "$t th try." . $br;
		$st = microtime();
		$ret = iword_set($str);
		$en = microtime();
		$st = explode(' ', $st);
		$st = (float)$st[0] + (float)$st[1];
		$en = explode(' ', $en);
		$en = (float)$en[0] + (float)$en[1];
		$tm = round(($en - $st) * 1000, 3);
		echo "D: $tm ms " . strlen($ret) . $br;
	}
	echo $br;
}
//*/

// テストここまで

// 後処理
echo "$br$br";

function test_start() {
	global $br; static $i = 0; $i++;
	echo "$br----(例{$i})----------------------------------------$br";
}
?>
