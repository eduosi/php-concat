php-concat
==========

php-concat 是 PHP 支持文件合并模块，主要用于合并前端代码减少 http 请求数，需要 Web Server 支持。

主要是解决了，在 Web Server 支持文件合并和不支持文件合并环境之间的兼容性问题。

如：开发环境使用原生 Nginx 作为 Web Server，生产环境使用 Tengine 并启用 ngx_http_concat_module 时。


Requirement
------------
PHP Version >= 5.3.0


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
$ ./configure --with-php-config=php_installed_path/bin/php-config [--enable-concat]
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
[Concat]
concat.enable = off 	boolean	default value is false. 是否启用文件合并
concat.prefix = "??" 	string 	default value is "??". 	分割前缀。
concat.delimiter = ","	string 	default value is ",". 	文件与文件之间的分隔符。
concat.max_files = 0 	integer default value is 0. 	最大合并文件数，0 为不限制。
```

Documentation
-------------
```
void Concat::setEnable(boolean $enable);
功能说明：设置是否启用文件合并
参数：boolean $enable 是否启用文件合并
返回值：无

boolean Concat::getEnable();
功能说明：返回是否启用文件合并
参数说明：无
返回值：boolean 是否启用文件合并

void Concat::setPrefix(string $prefix);
功能说明：设置分割前缀
参数：string $prefix 分割前缀
返回值：无

string Concat::getPrefix();
功能说明：返回分割前缀
参数说明：无
返回值：string 分割前缀

void Concat::setDelimiter(string $delimiter);
功能说明：设置文件分隔符
参数：string $delimiter 文件分隔符
返回值：无

string Concat::getDelimiter();
功能说明：返回文件分隔符
参数说明：无
返回值：string 文件分隔符

void Concat::setVersionPrefix(string $versionPrefix);
功能说明：设置文件版本前缀
参数：string $versionPrefix 文件版本前缀
返回值：无

string Concat::getVersionPrefix();
功能说明：返回文件版本前缀
参数说明：无
返回值：string 文件版本前缀

void Concat::setVersion(string $version);
功能说明：设置文件版本
参数：string $version 文件版本
返回值：无

string Concat::getVersion();
功能说明：返回文件版本
参数说明：无
返回值：string 文件版本

void Concat::setMaxFiles(integer $maxFiles);
功能说明：设置最大合并文件数
参数：integer $maxFiles 最大合并文件数
返回值：无

integer Concat::getMaxFiles();
功能说明：返回最大合并文件数
参数说明：无
返回值：integer 最大合并文件数

string Concat::css(string $baseUrl, mixed ...);
功能说明：css 文件合并
参数说明：string $baseUrl 基 URL
		 mixed 需要合并的文件名称
返回值：string 合并后的 style 标签

string Concat::javascript(string $baseUrl, mixed ...)
功能说明：js 文件合并
参数说明：string $baseUrl 基 URL
		 mixed 需要合并的文件名称
返回值：string 合并后的 script 标签

string Concat::js(string $baseUrl, mixed ...)
功能说明：Concat::javascript 别名
参数说明：string $baseUrl 基 URL
		 mixed 需要合并的文件名称
返回值：string 合并后的 script 标签
```

```
ini_set("concat.enable", "on");
echo Concat::css("http://www.example.com/css/", "a.css", "b/a.css");

// <link href="http://www.example.com/css/??a.css,b/a.css" rel="stylesheet" type="text/css" />

Concat::setVersionPrefix("version");
Concat::setVersion("0.1");

echo Concat::javascript("http://www.example.com/js/", "jquery.js", "jquery.ui.js");
// <script src="http://www.example.com/js/??jquery.js,jquery.ui.js?version=0.1" type="text/javascript"></script>


ini_set("concat.enable", "off");
echo Concat::css("http://www.example.com/css/", "a.css", "b/a.css");

// <link href="http://www.example.com/css/a.css" rel="stylesheet" type="text/css" /><link href="http://www.example.com/css/b.css" rel="stylesheet" type="text/css" />

Concat::setVersionPrefix("version");
Concat::setVersion("0.1");

echo Concat::javascript("http://www.example.com/js/", "jquery.js", "jquery.ui.js");

// <script src="http://www.example.com/js/jquery.js?version=0.1" type="text/javascript"></script><script src="http://www.example.com/js/jquery.ui.js?version=0.1" type="text/javascript"></script>
```