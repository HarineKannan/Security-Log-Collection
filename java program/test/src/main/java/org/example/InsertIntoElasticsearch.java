package org.example;

import org.apache.http.HttpHost;
import org.elasticsearch.action.index.IndexRequest;
import org.elasticsearch.client.RequestOptions;
import org.elasticsearch.client.RestClient;
import org.elasticsearch.client.RestHighLevelClient;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class InsertIntoElasticsearch {
    public static void insertion(Object[] result) throws IOException {
        RestHighLevelClient client = new RestHighLevelClient(
                RestClient.builder(new HttpHost("localhost", 9200, "http")));

        String indexName = "collectedlogs";

        try {
            for (Object obj : result) {
                Object[] array = (Object[]) obj;

                Map<String, Object> document = new HashMap<>();
                document.put("eventcode", array[0]);
                document.put("sourcename", array[1]);

                IndexRequest request = new IndexRequest(indexName).source(document);

                client.index(request, RequestOptions.DEFAULT);
                System.out.println("Document indexed successfully.");
            }
        } catch (IOException e) {
            System.err.println("Error indexing documents " );
        } finally {
                client.close();

        }
    }
}
