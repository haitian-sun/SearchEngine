package edu.umich.cse.eecs485;

import java.io.File;
import java.io.IOException;

import nu.xom.*;

public class XOMtest {
    public static void main(String[] args) throws ValidityException, ParsingException, IOException {
        //		String strdoc = "<eecs485_articles>" +
        //				"<eecs485_article>" +
        //				"<eecs485_article_id>303</eecs485_article_id>" +
        //				"<eecs485_article_title>Alabama</eecs485_article_title>" +
        //				"<eecs485_article_body>" +
        //				"    body" +
        //				"</eecs485_article_body>" +
        //				"</eecs485_article>" +
        //				"</eecs485_articles>";

        Builder builder = new Builder();

        //		Document doc = builder.build(strdoc, null);

        String filepath = "/Users/chjun/workspace/hadoop-1.0.1/dataset/mining.imageUrls.xml";
        File file = new File(filepath);

        Document doc = builder.build(file);
        //		Nodes nodeId = doc.query("//eecs485_image");
        //		System.out.println(nodeId.get(0).getChild(0).getValue());
    }
}
