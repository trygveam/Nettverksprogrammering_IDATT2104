package edu.ntnu.idatt2104.oving5;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

@SpringBootApplication
@RestController
public class Oving5Application {

	public static void main(String[] args) {
		SpringApplication.run(Oving5Application.class, args);
	}
}