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
		<li class="active"><a href="pull.php">Pull</a></li>
		<li><a href="post.php">Post</a></li>
		<li><a href="friends.php">Friends</a></li>
		<li><a href="https://apps.facebook.com/og_nafsocialmedia/" target="_blank">Canvas</a></li>
		<li><a href="youtube.html">Youtube</a></li>
	</ul></nav>
	
	</header><!-- /#banner -->
	
	
	<section id="content" class="body">
	<!-- Create PHP login session -->
	
	<?php 
	
	$app_id = "264231420378511";
	$app_secret = "d2027c6f159839424067957c21a5bc90";
	$my_url = "http://group21.naf.cs.hut.fi/ass2/pull.php";
	
	session_start();
	$_SESSION['userName'] = 'Root';
	
	
	/* Authenticate the user and additional permissions
		The additonal permissions is done adding the scope
		source: https://developers.facebook.com/docs/reference/api/user/ */
		
	$code = $_REQUEST["code"]; // Capture the code parameter and store it as a PHP variable
	
	if(empty($code)) {
	 $_SESSION['state'] = md5(uniqid(rand(), TRUE)); // CSRF protection
	 $dialog_url = "https://www.facebook.com/dialog/oauth?client_id=" 
	   . $app_id . "&redirect_uri=" . urlencode($my_url) . "&state="
	    . $_SESSION['state'] . "&scope=user_birthday,email,user_education_history,user_about_me";
	
	 header("Location: " . $dialog_url);
	}
	
	/*  Handle the response from the Login */
	if($_SESSION['state'] && ($_SESSION['state'] === $_REQUEST['state'])) {
	/*  Exchange the code for an Access Token that is to be able to use API */
	 $token_url = "https://graph.facebook.com/oauth/access_token?"
	   . "client_id=" . $app_id . "&redirect_uri=" . urlencode($my_url)
	   . "&client_secret=" . $app_secret . "&code=" . $code;
	
	 $response = file_get_contents($token_url);
	 $params = null;
	 parse_str($response, $params);
	 
	 
	 $_SESSION['access_token'] = $params['access_token'];
	
	 $graph_url = "https://graph.facebook.com/me?access_token=" . $params['access_token'];
	
	 /* Get the contents and stored in the variable $user  */
	 $user = json_decode(file_get_contents($graph_url));
	 
	 /* PULLING THE INFORMATION */
	 echo("Welcome " . $user->name);
	 echo("<p> </p>");
	 echo("Usename: " . $user->username);
	 $id =$user->id;
	 echo("<p> </p>");
	
	  ?>
	   	<img src="https://graph.facebook.com/<?= $id ?>/picture?width=90&height=100">
	  
	  <?php   
	 echo("<p></p>");
	 echo("Birthday: " . $user->birthday);
	 echo("<p></p>");
	 echo("Email: " . $user->email);
	 echo("<p></p>");
	 echo("Facebook Link: " . $user->link);
	 echo("<p></p>");
	 echo("_____________________________________"); 
	 echo("<p></p>");
	 echo("Education: ");
	 $education = $user->education ; 
	 $i = 1;
	echo("<p></p>");
	 if(!empty($education)){
	 foreach($education as $key=>$value){		
		      foreach($value as $key2=>$value2){
			      
			      // Name of the school	
			      	if (strcmp("school", $key2) == 0){
				          foreach($value2 as $key3=>$value3){
					      	
					        if (strcmp("name", $key3) == 0){   
					        echo(" <strong> <u> $i $key2 </u> </strong>");
					        echo("<p></p>");
					        echo("Name: $value3");
					        }         
				          }
				          echo("<p></p>");
				          $i++;
			      	}	
			      	 // Type
			      	 	
			      	 if (strcmp("type", $key2) == 0){
				      	  echo(ucfirst($key2) . ": $value2"); //ucfirst capitalize the first character
				      	  echo("<p></p>");
			      	 }
			      	 
			      	// Classes 	
			      	
			      	if (strcmp("classes", $key2) == 0){
				      	   foreach($value2 as $key3=>$value3){
				      	   		 foreach($value3 as $key4=>$value4){
					      	   		 if (strcmp("name", $key4) == 0){
				      	   		 		echo("	Year: $value4");
				      	   		 		echo("<p></p>");
				      	   		 	}
				      	   		 }
				      	   	}
					  }
		  }   
	}
	}
	else{
		echo("You don't have information on the field <u>EDUCATION</u>");
	}
	 echo("_____________________________________");
	 echo("<p></p>");   		 
	}
	else {
	 echo("The state does not match. You may be a victim of CSRF.");
	}
	
	?>
		
	</section>
	
	</body>
	</html>