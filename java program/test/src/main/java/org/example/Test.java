package org.example;
import org.example.InsertIntoElasticsearch;

import java.io.IOException;

public class Test {
    private native Object[] getArray();

    static {
        System.load("C:\\Users\\harine-pt7602\\source\\repos\\test\\x64\\Debug\\test.dll");
    }

    public static void main(String[] args) throws IOException,NullPointerException {
        Test sample = new Test();
        Object[] result = sample.getArray();
        for (Object obj : result) {
            Object[] array = (Object[]) obj;
            System.out.println("Event Code: " + array[0] + ", Source Name: " + array[1]);
        }
        InsertIntoElasticsearch.insertion(result);
    }
}
