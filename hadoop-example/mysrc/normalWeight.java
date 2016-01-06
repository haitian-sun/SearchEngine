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
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

import java.util.HashMap;
import nu.xom.*;
import java.util.Iterator;
import java.util.Map.Entry;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.lang.*;
//import java.text.*;



public class normalWeight
{
	private static Integer total = 0;

	public static class Map extends Mapper<LongWritable, Text, Text, Text> {
		public void map(LongWritable key, Text value, Context context)
				throws IOException, InterruptedException {

			String pages = value.toString();
			String[] pageList = pages.split("\\s+");

			String realKey = pageList[0];
				
			for (int i = 1; i < pageList.length; i = i + 2){
				context.write(new Text(pageList[i]), new Text(realKey + " " + pageList[i+1]));
			}
		}
	}

	public static class Reduce extends Reducer<Text, Text, Text, Text> {
		
		public void reduce(Text key, Iterable<Text> values, Context context)
				throws IOException, InterruptedException {
			
			double norm = 0;
			HashMap<String, Double> weights = new HashMap<String, Double>();

			for (Text value : values) {
				String words = value.toString();
				String[] wordList = words.split("\\s+");
				norm += Math.pow(Double.parseDouble(wordList[1]), 2);
				weights.put(wordList[0], Double.parseDouble(wordList[1]));
			}

			norm = Math.sqrt(norm);
			String invList = "";

			//DecimalFormat df = new DecimalFormat("#.####E0");

			for (Entry<String, Double> entry: weights.entrySet()) {
    			String word = entry.getKey();
    			Double w = entry.getValue();
				//invList = invList + word + " " + df.format(w/norm) + " ";
				invList = invList + word + " " + Double.toString(w/norm) + " ";
			}


			/*for (Text value: cache) {
				String words = value.toString();
				String[] wordList = words.split("\\s+");	
				double weight = Double.parseDouble(wordList[1])/norm;
				//SSystem.out.println(wordList[0]);
				invList = invList + wordList[0] + " " + Double.toString(weight) + " ";
			}*/

			context.write(key, new Text(invList));	
		}
	}

	public static void main(String[] args) throws Exception
	{
		Configuration conf = new Configuration();

		Job job = new Job(conf, "normalWeight");

		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(Text.class);

		job.setMapperClass(Map.class);
		job.setReducerClass(Reduce.class);

		job.setInputFormatClass(TextInputFormat.class);
		job.setOutputFormatClass(TextOutputFormat.class);

		FileInputFormat.addInputPath(job, new Path(args[0]));
		FileOutputFormat.setOutputPath(job, new Path(args[1]));

		job.waitForCompletion(true);
	}
}
