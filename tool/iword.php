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
