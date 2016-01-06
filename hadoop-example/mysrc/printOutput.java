package edu.umich.cse.eecs485;

import java.io.*;
import java.text.*;

public class printOutput{
    public static void main(String[] args) throws Exception{
        String fileName = args[0];
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

                System.out.printf("%s\t%d ",words[0], Integer.valueOf(words[1]));
                for (int i = 2; i < words.length; i = i + 2){
                     System.out.printf("%d:%.4e ", Integer.valueOf(words[i]), Double.valueOf(words[i+1]));
                }
                System.out.printf("\n");
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
