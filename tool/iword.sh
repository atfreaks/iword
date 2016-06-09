#!/bin/sh
# iWord動的ロードによるテスト
php -d extension_dir=modules -d extension=iword.so iword$1.php
