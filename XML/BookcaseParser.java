import java.io.*;

import javax.xml.transform.Source;
import javax.xml.transform.stream.StreamSource;
import javax.xml.validation.*;
import org.xml.sax.SAXException;
import org.xml.sax.*;
import org.w3c.dom.*;
import javax.xml.parsers.*;



public class BookcaseParser{
				
	public enum Command {
		LIST,
		VALIDATE
	}

	public static void main(String[] args){
				
		boolean isValid = false; 

		// In case the user introduces a wrong number of arguments
		if(args.length!=2){
			System.out.println("Wrong number of arguments. Sample command line: BookcaseParser xyz.xml validate");
			return;
		}

		String fileName = args[0];
		String command = args[1];

		try{
			Command givenCommand = Command.valueOf(command.toUpperCase());

				switch(givenCommand){
					case VALIDATE: xmlValidator(fileName);
									break;
					
					case LIST: isValid = xmlValidator(fileName);
								if(isValid)
									listing(fileName);
								break;
					
					default: System.out.println("Invalid command. Try 'validate' or 'list'.");
							break;
				}
			} catch (IllegalArgumentException iae){
				System.out.println("Illegal Arguments Exception: " + command + " does not exist." );
			} catch (Exception e) {
				System.out.println("Exception"+ fileName + " is not valid because "+ e.toString() );
			}	
		
		return ;
		}	

	// Validates the given xmlFile, using some functions from the XML Validation API
	// Sources:  www.docs.oracle.com (description)
	//			http://www.ibm.com/developerworks/library/x-javaxmlvalidapi/ (code) - with some changes
	public static boolean xmlValidator(String xmlFile) throws SAXException, IOException{
		
		boolean isValid = false;

		// 1 
		//Schema factory is a schema compiler.  It reads external representations of schemas and prepares them for validation.
		//Lookup an implementation of the SchemaFactory that supports the specified schema language and return it.
		//In this case e W3C XML Schema language
		SchemaFactory factory = SchemaFactory.newInstance("http://www.w3.org/2001/XMLSchema");
					
		// 2 
		//Put the schema (xsd) as a file for after get a schema object.
		//Schema object represents a set of constraints that can be checked/ enforced against an XML document .
		File schemaLocation = new File("Bookcase.xsd");
		Schema schema = factory.newSchema(schemaLocation);
					
		//3 
		//Creates a validator for this schema. 
		//A validator enforces/checks the set of constraints this object represents.
		Validator validator = schema.newValidator();
					
		// 4 
		//Parse the document you want to check.
		//StreamSource Acts as an holder for a transformation Source in the form of a stream of XML markup.
		File xmlDoc = new File(xmlFile);
		Source source = new StreamSource(xmlDoc);
					
		//5
		//Validate the xml file(input source) against the schema file.
		try{
		validator.validate(source);
		isValid = true; 
		
		} catch (SAXException se) {
			System.out.println("SAXException"+ xmlFile + " is not valid because "+ se.toString() );
		} catch (IOException ioe) {
			System.out.println("IOException"+ xmlFile + " is not valid because "+ ioe.toString() );
		} catch (Exception e) {
			System.out.println("Exception"+ xmlFile + " is not valid because "+ e.toString() );
		}	 

		if (isValid)
			System.out.println(xmlFile + " is valid.");

		return isValid;

	}

	// Function that lists the components defined in the XML File
    // Source: http://javarevisited.blogspot.com/2011/12/parse-xml-file-in-java-example-tutorial.html (code) - with some changes
	public static void listing (String xmlFile) throws SAXException, IOException {
		try{

			File xml = new File(xmlFile);
			DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
			DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
			
			//Parse the content of the file and return a Document 
			Document doc = dBuilder.parse(xml);
			//Puts all Text nodes in the full depth of the sub-tree underneath this Node,
			//including attribute nodes, into a "normal" form where only structure 
			doc.getDocumentElement().normalize();

			System.out.println("Contents of the bookcase: ");

			//Saves in a list the "Shelf" nodes (each node will represent each shelf)
			NodeList bookcaseNodes = doc.getElementsByTagName("bs:shelf");
			
			// Cycle of operations for each shelf
			for(int i = 0; i < bookcaseNodes.getLength() ; i++){
				Node shelfNodes = bookcaseNodes.item(i);
				Element nodeElem = (Element) shelfNodes;
				System.out.println("===" + nodeElem.getAttribute("bbt:shelfName").toUpperCase() + " SHELF ===");

				//In each shelf, there may exist books and magazines
				//The following block represents each book (which is saved in a node)
				NodeList booksNodesList = nodeElem.getElementsByTagName("bs:book");
				for (int j = 0; j < booksNodesList.getLength(); j++){
					Node bookNodes = booksNodesList.item(j);
					Element bookNodeElem = (Element) bookNodes;
					
					System.out.println(getValue("bbt:category", bookNodeElem).toUpperCase() + " [" + bookNodeElem.getAttribute("bbt:language") 
						+ "]: Title: " + getValue("bbt:title", bookNodeElem) + "; Author: " + getValue("bbt:author", bookNodeElem) 
						+ "; Publisher: " + getValue("bbt:publisher", bookNodeElem));
				}

				//Representation of the magazines
				NodeList magazinesNodesList = nodeElem.getElementsByTagName("bs:magazine");
				for (int j = 0; j < magazinesNodesList.getLength(); j++){
					Node magazineNodes = magazinesNodesList.item(j);
					Element magazineNodeElem = (Element) magazineNodes;
					System.out.println(getValue("bbt:category", magazineNodeElem).toUpperCase() + " [" + magazineNodeElem.getAttribute("bbt:language") 
						+ "]: Title: " + getValue("bbt:title", magazineNodeElem) + "; Publish Date: " + magazineNodeElem.getAttribute("bbt:date")
						+ "; Publisher: " + getValue("bbt:publisher", magazineNodeElem));
				}
				System.out.println("=============");
			}
			
		} catch (SAXException se) {
			System.out.println("SAXException: Cannot parse file "+ xmlFile);
		} catch (IOException ioe) {
			System.out.println("IOException: Cannot parse file "+ xmlFile);
		} catch (ParserConfigurationException pce) {
			System.out.println("ParserConfigurationException: Cannot parse file "+ xmlFile);
		}		    
	}

	// Auxiliar function of the list function - returns the value (in a String format) with the name "tag" from the element "element" 
    // Source: http://javarevisited.blogspot.com/2011/12/parse-xml-file-in-java-example-tutorial.html
	private static String getValue(String tag, Element element) {
		NodeList nodes = element.getElementsByTagName(tag).item(0).getChildNodes();
		Node node = (Node) nodes.item(0);
		return node.getNodeValue();
	}



}