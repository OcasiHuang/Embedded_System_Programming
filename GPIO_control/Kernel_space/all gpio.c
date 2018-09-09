

int err;
err=gpio_request(11,"GPIO00") 
printk("gpio %d is request \n", err);
err=gpio_request(32,"GPIO00_shifter") 
printk("gpio %d is request \n", err);

err=gpio_request(12,"GPIO01") 
printk("gpio %d is request \n", err);
err=gpio_request(28,"GPIO01_shifter") 
printk("gpio %d is request \n", err);
err=gpio_request(45,"GPIO01_mux") 
printk("gpio %d is request \n", err);
	
err=gpio_request(13,"GPIO02") 
printk("gpio %d is request \n", err);
err=gpio_request(34,"GPIO02_shifter") 
printk("gpio %d is request \n", err);
err=gpio_request(77,"GPIO02_mux") 
printk("gpio %d is request \n", err);

err=gpio_request(14,"GPIO03") 
printk("gpio %d is request \n", err);
err=gpio_request(16,"GPIO03_shifter") 
printk("gpio %d is request \n", err);
err=gpio_request(76,"GPIO03_mux") 
printk("gpio %d is request \n", err);

err=gpio_request(6,"GPIO04") 
printk("gpio %d is request \n", err);
err=gpio_request(36,"GPIO04_shifter") 
printk("gpio %d is request \n", err);


err=gpio_request(0,"GPIO05") 
printk("gpio %d is request \n", err);
err=gpio_request(18,"GPIO05_shifter") 
printk("gpio %d is request \n", err);
err=gpio_request(66,"GPIO05_mux") 
printk("gpio %d is request \n", err);
	
err=gpio_request(1,"GPIO06") 
printk("gpio %d is request \n", err);
err=gpio_request(20,"GPIO06_shifter") 
printk("gpio %d is request \n", err);
err=gpio_request(68,"GPIO06_mux") 
printk("gpio %d is request \n", err);

err=gpio_request(38,"GPIO07") 
printk("gpio %d is request \n", err);

err=gpio_request(40,"GPIO08") 
printk("gpio %d is request \n", err);


err=gpio_request(4,"GPIO09") 
printk("gpio %d is request \n", err);
err=gpio_request(22,"GPIO09_shifter") 
printk("gpio %d is request \n", err);
err=gpio_request(70,"GPIO09_mux") 
printk("gpio %d is request \n", err);

err=gpio_request(10,"GPIO10") 
printk("gpio %d is request \n", err);
err=gpio_request(26,"GPIO10_shifter") 
printk("gpio %d is request \n", err);
err=gpio_request(74,"GPIO10_mux") 
printk("gpio %d is request \n", err);

err=gpio_request(5,"GPIO11") 
printk("gpio %d is request \n", err);
err=gpio_request(24,"GPIO11_shifter") 
printk("gpio %d is request \n", err);
err=gpio_request(44,"GPIO11_mux") 
printk("gpio %d is request \n", err);
	
err=gpio_request(15,"GPIO12") 
printk("gpio %d is request \n", err);
err=gpio_request(42,"GPIO12_shifter") 
printk("gpio %d is request \n", err);

err=gpio_request(7,"GPIO13") 
printk("gpio %d is request \n", err);
err=gpio_request(30,"GPIO13_shifter") 
printk("gpio %d is request \n", err);
err=gpio_request(46,"GPIO13_mux") 
printk("gpio %d is request \n", err);
	
	
gpio_direction_output(11, 0);
gpio_direction_output(32, 0);
gpio_direction_output(12, 0);
gpio_direction_output(28, 0);
gpio_direction_output(45, 0);
gpio_direction_output(13, 0);
gpio_direction_output(34, 0);
gpio_direction_output(77, 0);
gpio_direction_output(14, 0);
gpio_direction_output(16, 0);
gpio_direction_output(76, 0);
gpio_direction_output(6, 0);
gpio_direction_output(36, 0);
gpio_direction_output(0, 0);
gpio_direction_output(18, 0);
gpio_direction_output(66, 0);
gpio_direction_output(1, 0);
gpio_direction_output(20, 0);
gpio_direction_output(68, 0);
gpio_direction_output(34, 0);
gpio_direction_output(40, 0);
gpio_direction_output(4, 0);
gpio_direction_output(22, 0);
gpio_direction_output(70, 0);
gpio_direction_output(10, 0);
gpio_direction_output(26, 0);
gpio_direction_output(74, 0);
gpio_direction_output(5, 0);
gpio_direction_output(24, 0);
gpio_direction_output(44, 0);
gpio_direction_output(15, 0);
gpio_direction_output(42, 0);
gpio_direction_output(7, 0);
gpio_direction_output(30, 0);
gpio_direction_output(46, 0);



