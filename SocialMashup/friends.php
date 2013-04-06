<! Source: http://coding.smashingmagazine.com/2009/08/04/designing-a-html-5-layout-from-scratch/ -->

<!DOCTYPE html>
<html lang="en-US"> 
  
 <head>     
<meta charset="utf-8" />
 <meta name="viewport" content="initial-scale=1.0, user-scalable=no" />
<title>NAF - Assignment 2</title>

<link rel="stylesheet" href="css/style.css" type="text/css" />

<!-- FQL source: https://developers.facebook.com/docs/technical-guides/fql/ -->

<?php
  $app_id = '264231420378511';
  $app_secret = 'd2027c6f159839424067957c21a5bc90';
  $my_url = 'http://group21.naf.cs.hut.fi/ass2/friends.php';
  
  	session_start();
	$_SESSION['userName'] = 'Root';

  $code = $_REQUEST["code"];

 // auth user
 if(empty($code)) {
    $_SESSION['state'] = md5(uniqid(rand(), TRUE)); // CSRF protection
    $dialog_url = 'https://www.facebook.com/dialog/oauth?client_id=' 
    . $app_id . '&redirect_uri=' . urlencode($my_url) ."&scope=user_status,user_photos,friends_status,friends_photos";	
     header("Location: " . $dialog_url);
    
    
    echo("<script>top.location.href='" . $dialog_url . "'</script>");
  }

  // get user access_token
  
  $token_url = 'https://graph.facebook.com/oauth/access_token?client_id='
    . $app_id . '&redirect_uri=' . urlencode($my_url) 
    . '&client_secret=' . $app_secret 
    . '&code=' . $code ;
  
  // response is of the format "access_token=AAAC..."
  $access_token = substr(file_get_contents($token_url), 13);

  echo '<pre>';
  
    /* This query gets the userid  the latitude, the longitude and the messsage from a post of my friends 
	    SELECT author_uid, latitude, longitude, message FROM location_post WHERE author_uid IN
 (SELECT uid2 FROM friend WHERE uid1=me())
    */
   
     $fql_query_url = 'https://graph.facebook.com/'
    . "fql?q=SELECT+author_uid,+latitude,+longitude,+message+FROM+location_post+WHERE+author_uid+IN+(SELECT+uid2+FROM+friend+WHERE+uid1=me())" 
    . '&access_token=' . $access_token;
  $fql_query_result = file_get_contents($fql_query_url);
  $fql_query_obj = json_decode($fql_query_result, true);
  
  /* Save the data in variables */
  
  $uid = array(); // Array of uid, users id's
  $lat = array(); // Array of latitude
  $log = array(); // Array of longitude
  $name = array(); // Name who post
  $post = array(); // Content of the post 

  /* To determine the stop condition, it will get the extension of the array(facebook query) */
  	foreach($fql_query_obj as $value){
		
		$friendscount = count($value);
		break;      	    	
	}

	/* Grants 5  text posts with 5 differents friends and 5 differents locations  */		    	
      function friends($uid,$fql_query_obj,$i,$lat,$log,$post,$friendscount){
    	    
	      	    	$flag = 0 ;
	      	    	      	    		      	    
	      	    	/* Stop Condition, end of the array */
	      	    	
	      	    	if($friendscount == ($i-1)){
		      	    	 
		      	    	 return array($uid,$lat,$log,$post);
	      	    	}
	      	    	
	      	    	/* Grants not appear the same friend in different posts */
	      	    	foreach($uid as $value){
		      	    	
		      	    	if( $value == $fql_query_obj['data'][$i]['author_uid']){
			      	    	$flag = 1 ;
			      	    	break ;
		      	    	}
	      	    	} 
	      	    
	      	    	/* To grant it will not appear 2 posts on the same location */
	      	    	for($j =0 ; $j < count($lat) ; $j++){
		      	    	
		      	    	if($lat[$j] == $fql_query_obj['data'][$i]['latitude'] and ($log[$j] == $fql_query_obj['data'][$i]['longitude']) ){
			      	    	$flag = 1 ;
			      	    	break ;
		      	    	}	
	      	    	} 

	      	    	/* If the user didn't add text content to the post chooses another  */
	      	  
	      	    	if(empty($fql_query_obj['data'][$i]['message']) ){
		      	    	$flag = 1 ;
	      	    	} 
	      	    	
	      	    	if($flag == 0){
		      	    	
		      	    	$uid[count($uid)] = $fql_query_obj['data'][$i]['author_uid'];
		      	    	$lat[count($lat)] = $fql_query_obj['data'][$i]['latitude'];  							
		      	    	$log[count($log)] = $fql_query_obj['data'][$i]['longitude'];  
		      	    	$post[count($post)] = $fql_query_obj['data'][$i]['message'];
		      	    	
		      	   }
		      	   $i++; 	

	      	    	if(count($uid) < 5){
		      	    	return friends($uid,$fql_query_obj,$i,$lat,$log,$post,$friendscount);
		      	    	
	      	    	}else{
		      	    	 return array($uid,$lat,$log,$post);
	      	    	}
	      	 }   		    	
	      	 	
	    	/* Call the function friends */
	    	list($uid,$lat,$log,$post) = friends($uid,$fql_query_obj,count($uid),$lat,$log,$post,$friendscount) ;	    	
	
	/* Iterates the strings posts and eliminating future problems */	   
    for($i=0;$i<count($uid);$i++){
	    
		/* To avoid problems of ' and '' transform to a specific code */  
		$post[$i] = htmlspecialchars($post[$i], ENT_QUOTES);
				    
		   
		 /* Avoiding problems with enters */
		 $post[$i] = str_replace("\n", '<br></br>', $post[$i]);
		 $post[$i] = str_replace("\r", '<br></br>', $post[$i]);
		  
    } 
    
     
    /* Switch - Query gets the friends names by uid */
    switch(count($uid)){
   		
   		case 0: break;
   		    
	    case 1 : $fql_query_urln = 'https://graph.facebook.com/'
    . "fql?q=SELECT+name,+uid+FROM+user+WHERE+uid=$uid[0]"
    . '&access_token=' . $access_token; break; 
	    
	    case 2 : $fql_query_urln = 'https://graph.facebook.com/'
    . "fql?q=SELECT+name,+uid+FROM+user+WHERE+uid=$uid[0]+OR+uid=$uid[1]"
    . '&access_token=' . $access_token; break;
	    
	    case 3 : $fql_query_urln = 'https://graph.facebook.com/'
    . "fql?q=SELECT+name,+uid+FROM+user+WHERE+uid=$uid[0]+OR+uid=$uid[1]+OR+uid=$uid[2]"
    . '&access_token=' . $access_token; break;
	    
	    
	    case 4 : $fql_query_urln = 'https://graph.facebook.com/'
    . "fql?q=SELECT+name,+uid+FROM+user+WHERE+uid=$uid[0]+OR+uid=$uid[1]+OR+uid=$uid[2]+OR+uid=$uid[3]"
    . '&access_token=' . $access_token; break;
	    
	    case 5 :  $fql_query_urln = 'https://graph.facebook.com/'
    . "fql?q=SELECT+name,+uid+FROM+user+WHERE+uid=$uid[0]+OR+uid=$uid[1]+OR+uid=$uid[2]+OR+uid=$uid[3]+OR+uid=$uid[4]"
    . '&access_token=' . $access_token; break;
	    
    }
        
  $fql_query_resultn = file_get_contents($fql_query_urln);
  $fql_query_objn = json_decode($fql_query_resultn, true);
 
  /* Put the names in a decent array */
 for($i =0 ; $i < count($uid) ; $i++){
 
 	for($j =0 ; $j < count($uid) ; $j++){
	 	
	 	if(	$uid[$i] == $fql_query_objn['data'][$j]['uid']){
		 	
		 	$name[$i] = $fql_query_objn['data'][$j]['name'];  
		 	break;
	 	}
 	}
 }
 

	    	
  echo '</pre>'; 
?>
   
    <!--  Loading the Google Maps API  and Initialize the map (map initialization variables) -->
    
    <!-- source: https://developers.google.com/maps/documentation/javascript/tutorial -->
    
      <script type="text/javascript"
      src="https://maps.googleapis.com/maps/api/js?key=AIzaSyANjmQxnmqoDDcmdntCorCjr7walscBKik&sensor=true">
    </script>          
	          
     <script type="text/javascript"> 
		
		  function initialize() {
 
	      /* Coordinates from the friends posts  */
	    
	    /* Saves the php variable to the javascript variable  */
	    
	      var lat=new Array(); 
	      var log=new Array();
	      var name=new Array();
	      var message=new Array();
	      /* parseInt transforms string to int, nmarkers is the number of markers we will have */
	      var nmarkers = parseInt('<?php echo count($uid); ?>') ;
	      var Latlng=new Array(); 
	    
	     /* If doesn't have five markers popup alert */
	          if(nmarkers != 5){
		     alert("Unfortunately, the facebook query could not give you 5 different friends in five different locations with text content. We only could get: " + nmarkers + " of this/those" );
	     } 
	 
	 /* Store the data in javascript variables from php */
	     switch(nmarkers){
	     
	     case 5: lat[4] = '<?php echo $lat[4]; ?>' ;
	     		 log[4] = '<?php echo $log[4]; ?>' ;
	     	 	 name[4] = '<?php echo $name[4]; ?>';
	     	  	 message[4] = '<?php echo $post[4]; ?>' ;
	     		 Latlng[4] = new google.maps.LatLng(lat[4],log[4]);
		 
		case 4: lat[3] = '<?php echo $lat[3]; ?>' ;
	     		log[3] = '<?php echo $log[3]; ?>' ;
	     		name[3] = '<?php echo $name[3]; ?>';
	     		message[3] = '<?php echo $post[3]; ?>' ;
	     		Latlng[3] = new google.maps.LatLng(lat[3],log[3]);

	 
	   case 3:	lat[2] = '<?php echo $lat[2]; ?>' ;
	     		log[2] = '<?php echo $log[2]; ?>' ;
	     		name[2] = '<?php echo $name[2]; ?>';
	     		message[2] = '<?php echo $post[2]; ?>' ;
	     		Latlng[2] = new google.maps.LatLng(lat[2],log[2]);
	    
	    case 2:  lat[1] = '<?php echo $lat[1]; ?>' ;
	     		 log[1] = '<?php echo $log[1]; ?>' ;
	     		 name[1] = '<?php echo $name[1]; ?>';
	     		 message[1] = '<?php echo $post[1]; ?>' ;
	     		 Latlng[1] = new google.maps.LatLng(lat[1],log[1]);

	 
	   case 1:	lat[0] = '<?php echo $lat[0]; ?>' ;
	     		log[0] = '<?php echo $log[0]; ?>' ;
	     		name[0] = '<?php echo $name[0]; ?>';
	     		message[0] = '<?php echo $post[0]; ?>' ; 		
	     		Latlng[0] = new google.maps.LatLng(lat[0],log[0]);
	
	     } 
	 
	      
        var mapOptions = {   
	        /* CENTER: Coordinates of the center of the earth */
	      center: new google.maps.LatLng(0,78),  
          zoom: 1,
        mapTypeId: google.maps.MapTypeId.HYBRID 
          
        };
        var map = new google.maps.Map(document.getElementById("map-canvas"), mapOptions);
        
        var baloon = new Array();
    
      	var infowindow = new google.maps.InfoWindow({});
  
      
    for(var i=0; i<nmarkers ; i++){
	 	 
	  baloon[i] = 
   
    '<h2><b>Post by: </b>' + name[i] + '</h2>'+
    
     '<p><u>Coordinates:</u>'+ ' Latitude: '+ lat[i] +' and ' + 'Longitude: ' + log[i] + '</p>' +
    
    '<p><b>Content of the post: </b>' + message[i] + '</p>';  
    
     createMarkers(i,baloon[i]);     
    }
      
 	function createMarkers(i,baloon){
		  
		  var marker = new google.maps.Marker({
		    	 map: map,
		    	 position: Latlng[i]
		    	 });   
		    	 
		    	 	 google.maps.event.addListener(marker, 'click', function() {
			    	 infowindow.setContent(baloon);
			    	 infowindow.open(map,this);   	
			    });	 
	  }
  } 
  		      
   </script>
</head>
<body id="index" class="friends" onload="initialize()">


<header id="banner" class="body">
	<h1>Social Media and Mashups</h1>
 
	<nav><ul>
		<li ><a href="index.html">Home</a></li>
		<li><a href="contact.html">Contact</a></li>
		<li><a href="pull.php">Pull</a></li>
		<li><a href="post.php">Post</a></li>
		<li class="active"><a href="friends.php">Friends</a></li>
		<li><a href="https://apps.facebook.com/og_nafsocialmedia/" target="_blank">Canvas</a></li>
		<li><a href="youtube.html">Youtube</a></li>
	</ul></nav>
 
</header>


<section id="content" class="body">
 
 <article class="hentry">	
			<header>
				<h2 class="entry-title">Posts locations of your friends:</h2>
			</header>
 
			<div class="entry-content">
				<p> The map will show 5 locations of posts from 5 different friends in five different locations </p>
	
<!-- Google Maps JavaScript API -->

<div id="map-canvas"  style="width: 640px; height: 480px;">map div</div>

				
 </div>
</article> 
</section>
</body>
</html>