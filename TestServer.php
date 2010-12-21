<?php
/*
 *  ImageUpload
 *
 *  Copyright (C) 2010  Volker Grabsch <vog@m-click.de>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  <http://www.gnu.org/licenses/#GPL>
 */

$image = file_get_contents('php://input');

if ($image == '') {
    $this_url = (isset($_SERVER["HTTPS"]) ? 'https' : 'http').'://'.(isset($_SERVER['PHP_AUTH_USER']) ? $_SERVER['PHP_AUTH_USER'].':'.$_SERVER['PHP_AUTH_PW'].'@' : '').$_SERVER['HTTP_HOST'].$_SERVER['REQUEST_URI'];
    $upload_url = 'data:application/x-image-upload,'.rawurlencode($this_url);
    echo '<a href="'.$upload_url.'">ImageUpload</a>';
    echo '<textarea style="width: 90%; height: 10em;" onchange="document.getElementById(\'content\').innerHTML = this.value"></textarea>';
    echo '<p id="content">';
} else {
    $filename = 'img_'.sha1($image).'.png';
    file_put_contents($filename, $image);
    header('Content-type: text/plain');
    echo '<img src="'.$filename.'">';
}
