
 <! Source: http://coding.smashingmagazine.com/2009/08/04/designing-a-html-5-layout-from-scratch/ -->

<!DOCTYPE html>
<html lang="en-US"> 
      
<meta charset="utf-8" />
<title>NAF - Assignment 2</title>

<link rel="stylesheet" href="css/style.css" type="text/css" />

</head>
 
<body id="index" class="contact">

	


<header id="banner" class="body">
	<h1>Social Media and Mashups</h1>
 
	<nav><ul>
		<li><a href="index.html">Home</a></li>
		<li><a href="contact.html">Contact</a></li>
		<li><a href="pull.php">Pull</a></li>
		<li class="active"><a href="post.php">Post</a></li>
		<li><a href="friends.php">Friends</a></li>
		<li><a href="https://apps.facebook.com/og_nafsocialmedia/" target="_blank">Canvas</a></li>
		<li><a href="youtube.html">Youtube</a></li>
	</ul></nav>
 
</header>


<section id="content" class="body">

<br></br>

 <header>
	<h2 class="entry-title"> 1- Post photos on your Wall</h2>
</header>
 
 <!-- source: https://developers.facebook.com/blog/post/498/ -->
<?php

       $app_id = "264231420378511";
       $app_secret = "d2027c6f159839424067957c21a5bc90";
       $post_login_url = "http://group21.naf.cs.hut.fi/ass2/post.php";
       $album_name = 'NAF';
       $album_description = 'Testing upload photos ';

       $code = $_REQUEST["code"];

       //Obtain the access_token with publish_stream permission 
       if(empty($code))
         {
           $dialog_url= "http://www.facebook.com/dialog/oauth?"
           . "client_id=" . $app_id 
           . "&redirect_uri=" . urlencode($post_login_url)
           . "&scope=publish_stream";
           echo("<script>top.location.href='" . $dialog_url . 
           "'</script>");
       } 
       else {
         $token_url= "https://graph.facebook.com/oauth/"
         . "access_token?"
         . "client_id=" .  $app_id 
         . "&redirect_uri=" . urlencode( $post_login_url)
         . "&client_secret=" . $app_secret
         . "&code=" . $code;
         $response = file_get_contents($token_url);
         $params = null;
         parse_str($response, $params);
         $access_token = $params['access_token'];

         // Create a new album
         $graph_url = "https://graph.facebook.com/me/albums?"
         . "access_token=". $access_token;
   
         $postdata = http_build_query(
         array(
          'name' => $album_name,
          'message' => $album_description
            )
          );
         $opts = array('http' =>
         array(
          'method'=> 'POST',
          'header'=>
            'Content-type: application/x-www-form-urlencoded',
          'content' => $postdata
          )
         );
         $context  = stream_context_create($opts);
         $result = json_decode(file_get_contents($graph_url, false, 
           $context));

         // Get the new album ID
         $album_id = $result->id;

         //Show photo upload form and post to the Graph URL
         $graph_url = "https://graph.facebook.com/". $album_id
           . "/photos?access_token=" . $access_token;
         echo '<form enctype="multipart/form-data" action="'
         .$graph_url. ' "method="POST">';
         echo 'A photo will be added to the album ' . $album_name . '<br/><br/>';
         echo 'Please choose a photo: ';
         echo '<input name="source" type="file"><br/><br/>';
         echo 'Description of the photo: ';
         echo '<input name="message" type="text"
            value=""><br/><br/>';
         echo '<input type="submit" value="Upload"/><br/>';
         echo '</form>';
  
      }
 ?>

<br></br>
 
	 <header>
	<h2 class="entry-title"> 2 - Post a comment to your wall by our NAF application</h2>
</header>
	
	<FORM>
		<INPUT Type="BUTTON" VALUE="Post on Facebook" ONCLICK="window.location.href='wallpost.php'">
		</FORM>
 
</section>




</body>
</html>
 
