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

$data = '';
for ($i = 0; $i < 1024 * 1024; $i += 32)
 $data .= md5(rand());

for ($i = 0; $i < 10; $i++) {

$start = microtime();
iword_set($data);
$end = microtime();

$start = explode(' ', $start);
$end = explode(' ', $end);
$time = $end[0] + $end[1] - $start[0] - $start[1];
echo round(1024 / $time) . "KByte/sec\n";

}

?>
