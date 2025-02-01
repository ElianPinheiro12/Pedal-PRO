package com.example.pedalpro12;

import android.content.Intent;
import android.os.Bundle;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import android.widget.Button;
import android.widget.EditText;

import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import java.io.IOException;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.Request;
import okhttp3.Response;

import android.widget.TextView;

import okhttp3.OkHttpClient;

import android.content.SharedPreferences;

public class SettingsActivity extends AppCompatActivity {

    private Button btnConectar;
    private EditText editText;
    private TextView txtResposta;
    private final OkHttpClient client = new OkHttpClient();

    private SharedPreferences prefs;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_settings);

        btnConectar = findViewById(R.id.btnConectar2);
        editText = findViewById(R.id.editTextip2);
        txtResposta = findViewById(R.id.txtResposta2);

        prefs = getSharedPreferences("MEUS_DADOS", MODE_PRIVATE);
        editText.setText(prefs.getString("NOME", ""));



        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });



        Toolbar toolbar = findViewById(R.id.toolbar3);
        setSupportActionBar(toolbar);

        if (getSupportActionBar() != null) {
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setTitle("Configurações"); // Define o título
        }

        btnConectar.setOnClickListener(v -> obterDadosDoESP());
    }

    @Override
    public boolean onSupportNavigateUp() {
        Intent intent = new Intent(this, MainActivity.class);
        startActivity(intent);
        finish(); // Fecha a tela e volta para a anterior
        return true;
    }

    private void obterDadosDoESP() {
        String texto = editText.getText().toString();


        if (texto.isEmpty()) {
            txtResposta.setText("Por favor, insira um endereço IP.");
            return;
        }

        Request request = new Request.Builder().url("http://"+texto).build();


        SharedPreferences.Editor editor = prefs.edit();
        editor.putString("NOME", texto);
        editor.apply(); // Salva os dados


        client.newCall(request).enqueue(new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                runOnUiThread(() -> txtResposta.setText(texto +" Erro de conexão: " + e.getMessage()));
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                if (response.isSuccessful()) {
                    String resposta = response.body().string();
                    runOnUiThread(() -> txtResposta.setText("Resposta: " + resposta));
                }
            }
        });
    }
}
