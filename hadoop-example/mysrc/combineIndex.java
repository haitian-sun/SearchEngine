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
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.lang.*;



public class combineIndex
{
	public static class Map extends Mapper<LongWritable, Text, Text, Text> {
		public void map(LongWritable key, Text value, Context context)
				throws IOException, InterruptedException {

			String words = value.toString();
			String[] wordList = words.split("\\s+");

			String pageid = wordList[0];
				
			for (int i = 1; i < wordList.length; i = i + 2){
				context.write(new Text(wordList[i]), new Text(pageid+ " " + wordList[i+1]));
			}
		}
	}

	public static class Reduce extends Reducer<Text, Text, Text, Text> {
		
		public void reduce(Text key, Iterable<Text> values, Context context)
				throws IOException, InterruptedException {

			Integer DF = 0;
			String invList = "";

			for (Text value: values){	
				DF++;	
				String pages = value.toString();
				String[] pageList = pages.split("\\s+");
				invList = invList + pageList[0] + " " + pageList[1] + " ";
			}
			context.write(key, new Text(DF + " " + invList));
		}
	}

	public static void main(String[] args) throws Exception
	{
		Configuration conf = new Configuration();

		Job job = new Job(conf, "combineIndex");

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
