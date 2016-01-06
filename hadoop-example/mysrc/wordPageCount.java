package edu.umich.cse.eecs485;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.mahout.classifier.bayes.XmlInputFormat;
import java.util.HashMap;
import java.util.Map.Entry;
import nu.xom.*;
import java.util.Iterator;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.lang.*;

import edu.umich.cse.eecs485.filter;


public class wordPageCount
{
	private static Integer total = 0;

	public static class Map extends Mapper<LongWritable, Text, Text, LongWritable> {
		public void map(LongWritable key, Text value, Context context)
				throws IOException, InterruptedException {

			String strId = "";
			String strBody = "";

			// Parse the xml and read data (page id and article body)
			// Using XOM library
			Builder builder = new Builder();
			filter.initializeFilter();

			try {
				//System.out.println(value.toString());
				Document doc = builder.build(value.toString(), null);

				Nodes nodeId = doc.query("//eecs485_article_id");
				strId = nodeId.get(0).getChild(0).getValue();
				
				Nodes nodeBody = doc.query("//eecs485_article_body");
				strBody = nodeBody.get(0).getChild(0).getValue();
			}
			// indicates a well-formedness error
			catch (ParsingException ex) { 
				System.out.println("Not well-formed.");
				System.out.println(ex.getMessage());
			}  
			catch (IOException ex) {
				System.out.println("io exception");
			}
			
			// Tokenize document body
			Pattern pattern = Pattern.compile("\\w+");
			Matcher matcher = pattern.matcher(strBody);
			
			while (matcher.find()) {
				// Write the parsed token
				String word = new String(matcher.group());
				word = word.toLowerCase();
				if (!filter.isFilteredString(word))
					context.write(new Text(word), new LongWritable(Integer.valueOf(strId)));
			}
		}
	}

	public static class Reduce extends Reducer<Text, LongWritable, Text, Text> {
		
		public void reduce(Text key, Iterable<LongWritable> values, Context context)
				throws IOException, InterruptedException {
			
			// use a set to keep a set of unique doc IDs
			HashMap<String, Integer> count = new HashMap<String, Integer>();
			
			for (LongWritable value : values) {
				String pageid = String.valueOf(value.get());
				if (count.containsKey(pageid)){
					Integer num = count.get(pageid);
					count.put(pageid, num + 1);
				}
				else
					count.put(pageid, 1);
			} 

			int N_k = count.size();
			double idf_k = Math.log10((double)total/(double)N_k);

			// Convert the contents of the set into a list
			String invList = "";

			for (Entry<String, Integer> entry: count.entrySet()) {
    			String pageid = entry.getKey();
    			Integer tf_ik = entry.getValue();
    			double weight = (double)tf_ik.intValue() * idf_k;
				invList = invList + pageid + " " + Double.toString(weight) + " ";
			}

			context.write(key, new Text(invList));
		}
	}

	public static void main(String[] args) throws Exception
	{
		filter.setTotalPages("total");
		total = filter.total;

		Configuration conf = new Configuration();

		conf.set("xmlinput.start", "<eecs485_article>");
		conf.set("xmlinput.end", "</eecs485_article>");

		Job job = new Job(conf, "XmlParser");

		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(LongWritable.class);

		job.setMapperClass(Map.class);
		job.setReducerClass(Reduce.class);

		job.setInputFormatClass(XmlInputFormat.class);
		job.setOutputFormatClass(TextOutputFormat.class);

		FileInputFormat.addInputPath(job, new Path(args[0]));
		FileOutputFormat.setOutputPath(job, new Path(args[1]));

		job.waitForCompletion(true);
	}
}
