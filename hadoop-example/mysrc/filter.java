package edu.umich.cse.eecs485;

import java.io.*;
import java.util.HashSet;
import java.util.Iterator;


public class filter{
	private static HashSet<String> stopWords = new HashSet<String>();
    public static Integer total = 0;

	public static void initializeFilter(){
        String fileName = "../english.stop";
        String line = null;

        try {
            FileReader fileReader = new FileReader(fileName);
            BufferedReader bufferedReader = new BufferedReader(fileReader);

            while((line = bufferedReader.readLine()) != null) {
            	stopWords.add(line.toLowerCase());
            }   
            bufferedReader.close();         
        }
        catch(FileNotFoundException ex) {
            System.out.println("Unable to open file '" + fileName + "'");                
        }
        catch(IOException ex) {
            System.out.println("Error reading file '" + fileName + "'");                 
        }
	}


	public static boolean isFilteredString(String word){
		return stopWords.contains(word);
	}


    public static void setTotalPages(String target){
                // The name of the file to open.
        String fileName = target + ".txt";
        // This will reference one line at a time
        String line = null;

        try {
            // FileReader reads text files in the default encoding.
            FileReader fileReader = new FileReader(fileName);
            // Always wrap FileReader in BufferedReader.
            BufferedReader bufferedReader = 
                new BufferedReader(fileReader);

            while((line = bufferedReader.readLine()) != null) {
                String[] words = line.split("\\s+");
                total = Integer.parseInt(words[words.length - 1]);
            }   

            // Always close files.
            bufferedReader.close();         
        }
        catch(FileNotFoundException ex) {
            System.out.println("Unable to open file '" + fileName + "'");                
        }
        catch(IOException ex) {
            System.out.println("Error reading file '" + fileName + "'");                  
        }
    }
}
