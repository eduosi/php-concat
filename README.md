php-concat
==========

php-concat 是 PHP 支持文件合并模块，主要用于合并前端代码减少 http 请求数，需要 Web Server 支持。

主要是解决了，在 Web Server 支持文件合并和不支持文件合并环境之间的兼容性问题。

如：开发环境使用原生 Nginx 作为 Web Server，生产环境使用 Tengine 并启用 ngx_http_concat_module 时。


Requirement
------------
PHP Version >= 5.33


Download
------------
https://github.com/eduosi/php-concat/releases

latest source code:

git clone ssh://git@github.com:eduosi/php-concat.git


Installation
------------
```
$ cd php-concat
$ php_installed_path/bin/phpize
$ ./configure --with-php-config=php_installed_path/bin/php-config --enable-concat-debug=1/0(可选，是否开启 DEBUG，默认未开启，正式环境不建议开启)
$ make
# make install
```

最后，在php.ini中加入extension=concat.so


Configuration
------------
```
extension=php_concat.dll	; Windows
extension=concat.so			; *nux
```

```
concat.enable boolean default value is false。是否启用文件合并
concat.prefix string default value is "??"。
concat.delimiter string default value is ","。文件与文件之间的分隔符。
concat.max_files integer default value is 0. 最大合并文件数，0 为不限制。
```

Documentation
-------------
```
void Concat::setEnable(boolean $enable);
boolean Concat::getEnable();
void Concat::setPrefix(string $prefix);
string Concat::getPrefix();
void Concat::setDelimiter(string $delimiter);
string Concat::getDelimiter();
void Concat::setVersionPrefix(string $versionPrefix);
string Concat::getVersionPrefix();
void Concat::setVersion(string $version);
string Concat::getVersion();
void Concat::setMaxFiles(integer $maxFiles);
integer Concat::getMaxFiles();
string Concat::css(string $baseUrl, mixed ...);
string Concat::javascript(string $baseUrl, mixed ...)
string Concat::js(string $baseUrl, mixed ...)
```

```
ini_set("concat.enable", on);
echo Concat::css("http://www.example.com/css/", "a.css", "b/a.css");
// http://www.example.com/css/??a.css,b/a.css

Concat::setVersionPrefix("version");
Concat::setVersion("0.1");

echo Concat::javascript("http://www.example.com/js/", "jquery.js", "jquery.ui.js");
// http://www.example.com/js/??jquery.js,jquery.ui.js?version=0.1
```