<?php 

         $app_id = "264231420378511";

         $canvas_page = "https://apps.facebook.com/og_nafsocialmedia/";

         $feed_url = "https://www.facebook.com/dialog/feed?app_id=" 
                . $app_id . "&redirect_uri=" . urlencode($canvas_page) ;

         if (empty($_REQUEST["post_id"])) {    
            echo("<script> top.location.href='" . $feed_url . "'</script>");
         } else {
            echo ("Feed Post Id: " . $_REQUEST["post_id"]);
         }
?>