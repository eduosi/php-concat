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


后续
---
此项目毕竟[@eduosi](https://github.com/eduosi)是3年前写的,折腾的可能性非常低。遂fork出来顺便添加个帮助函数
`注意` 代码不能直接使用。需要修改require_once的载入路径。

```php

/**
 * 封装函数concat 使用WebServer concat模块合并静态文件请求
 *
 * 函数直接回显出html代码
 *
 * 目的：快捷生成静态文件的合并请求地址
 * 如：
 * <script href="//g.esc.cn/admin/aa/cheThird/??aa/bb.js,asd.js" type="text/javascript"></script>
 *
 * 普通调用：
 * concat('admin/aa/cheThird/','bb.css','bb.js','asd.js'); //js css可混用加载 函数内部已经分开处理
 * 输出：
 * <link href="//g.esc.cn/admin/aa/cheThird/??bb.css" rel="stylesheet" type="text/css" />
 * <script href="//g.esc.cn/admin/aa/cheThird/??bb.js,asd.js" type="text/javascript"></script>
 *
 *
 *
 * 独立配置调用：
 * Concat::setEnable(0);
 * Concat::setVersion(1);
 * Concat::setVersionPrefix('50');
 * concat('admin/aa/cheThird/','bb.css','aa/bb.js','asd.js');
 * 输出：
 * <link href="//g.esc.cn/admin/aa/cheThird/bb.css?50=1" rel="stylesheet" type="text/css" />
 * <script href="//g.esc.cn/admin/aa/cheThird/aa/bb.js?50=1" type="text/javascript"></script><script href="//g.esc.cn/admin/aa/cheThird/asd.js?50=1" type="text/javascript"></script>
 *
 *
 * 参数说明：
 * 参数1：路径地址，可以不需要域名,默认取值STYLE_URL常量的域名值；若传入域名则使用传入的值。这里会把/http[s]?:/ 匹配值去掉 使用相对协议
 * 参数2~：之后的参数就是静态文件名称，按照Nginx的concat模块的使用
 *
 * Concat类参考：
 * @URL https://github.com/Hootrix/php-concat
 * @Author Teng.Yong
 *
 */

if (!function_exists('concat')) {
    require_once APPPATH . 'libraries' . DIRECTORY_SEPARATOR . 'Concat.php';
    function concat($concatParams)
    {
        $styleUrl  = str_replace(array('https:','http:'),'',STYLE_URL);
        $enable = Concat::getEnable();
        if(!isset($enable) ) Concat::setEnable(true);
        $args = func_get_args();
        $args[0] = ltrim($args[0],'/') ;
        if(strpos($args[0],ltrim($styleUrl,'//')) !== false){//传入的参数1 存在当前设置的 STYLE_URL 域名 则删除协议前缀 使用相对协议
            $args[0] = ltrim($args[0],'http:') ;
            $args[0] = ltrim($args[0],'https:') ;
        }else{//否则插入域名
            if(strpos($args[0],'.') ===false ){//不是域名
                $args[0] = $styleUrl.$args[0];
            }else{//传入域名
                $args[0] = ltrim($args[0],'http:');
                $args[0] = ltrim($args[0],'https:');
                $args[0] = ltrim($args[0],'//') ;
                $args[0] = '//'.$args[0];
            }
        }

        $css = array();
        $js = array();
        for ($i = 1;$i<count($args);$i++){
            //判断传入的文件后缀
            if(strrchr($args[$i], '.css') === '.css'){
                $css []= $args[$i];
            }else{
                $js []= $args[$i];
            }
        }
        if (!empty($css)) {
            array_splice($css,0,0,$args[0]);
            echo call_user_func_array("Concat::css", $css), "\n";
        }
        if (!empty($js)) {
            array_splice($js,0,0,$args[0]);
            echo call_user_func_array("Concat::js", $js), "\n";
        }
    };
}
```
