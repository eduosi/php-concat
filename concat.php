<?php
	class Concat {

		private static $enable = NULL;

		private static $prefix = "??";

		private static $delimiter = ",";

		private static $versionPrefix = NULL;

		private static $version = NULL;

		private static $maxFiles = 0;

		public static function setEnable($enable){
			self::$enable = (boolean) $enable;
		}

		public static function getEnable(){
			return self::$enable;
		}

		public static function setPrefix($prefix){
			self::$prefix = (string) $prefix;
		}

		public static function getPrefix(){
			return self::$prefix;
		}

		public static function setDelimiter($delimiter){
			self::$delimiter = (string) $delimiter;
		}

		public static function getDelimiter(){
			return self::$delimiter;
		}

		public static function setVersionPrefix($versionPrefix){
			self::$versionPrefix = (string) $versionPrefix;
		}

		public static function getVersionPrefix(){
			return self::$versionPrefix;
		}

		public static function setVersion($version){
			self::$version = (string) $version;
		}

		public static function getVersion(){
			return self::$version;
		}

		public static function setMaxFiles($maxFiles){
			self::$maxFiles = (int) $maxFiles;
		}

		public static function getMaxFiles(){
			return self::$maxFiles;
		}

		public static function css($baseUrl){
            $baseUrl = rtrim($baseUrl,'/');
            $baseUrl .= '/';
			$args = func_get_args();
			$size = func_num_args() - 1;
			$end = self::$enable == false||self::$maxFiles == 0 ? $size : min(self::$maxFiles, $size);
			$version = self::$versionPrefix ? "?".self::$versionPrefix."=".(self::$version ? self::$version : "") : "";
			$i = 0;
			$url = "";

			if(self::$enable == true){
				$url = '<link href="'.$baseUrl.'??';

				foreach($args as $j=>$arg){
					if($i >= $end){
						break;
					}

					if($j > 0){
						$url .= ($i > 0 ? "," : "").(string) $arg;
						++$i;
					}
				}

				$url .= $version.'" rel="stylesheet" type="text/css" />';
			}else{
				foreach($args as $j=>$arg){
					if($j > 0){
						$url .= '<link href="'.$baseUrl.$arg.$version.'" rel="stylesheet" type="text/css" />';
					}
				}
			}

			return $url;
		}

		public static function javascript($baseUrl){
            $baseUrl = rtrim($baseUrl,'/');
            $baseUrl .= '/';
			$args = func_get_args();
			$size = func_num_args() - 1;
			$end = self::$enable == false||self::$maxFiles == 0 ? $size : min(self::$maxFiles, $size);
			$version = self::$versionPrefix ? "?".self::$versionPrefix."=".(self::$version ? self::$version : "") : "";
			$i = 0;
			$url = "";

			if(self::$enable == true){
				$url = '<script href="'.$baseUrl.'??';

				foreach($args as $j=>$arg){
					if($i >= $end){
						break;
					}

					if($j > 0){
						$url .= ($i > 0 ? "," : "").$arg;
						++$i;
					}
				}

				$url .= $version.'" type="text/javascript"></script>';
			}else{
				foreach($args as $j=>$arg){
					if($j > 0){
						$url .= '<script href="'.$baseUrl.$arg.$version.'" type="text/javascript"></script>';
					}
				}
			}

			return $url;
		}

		public static function js($baseUrl){
			$args = func_get_args();
			return call_user_func_array(array(__CLASS__, "javascript"), $args);
		}

	}