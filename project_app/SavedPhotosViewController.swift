//
//  SavedPhotosViewController.swift
//  project_app
//
//  Created by Arina Goloubitskaya on 15.03.20.
//  Copyright © 2020 Arina Goloubitskaya. All rights reserved.
//

import UIKit


class SavedPhotosViewController: UIViewController, UITextFieldDelegate {

    
    //возвращаемся обратно в меню
    @IBAction func BackToMenuButton(_ sender: Any) {
        self.dismiss(animated: true, completion: nil)
    }
    
    var num: Int!
    var text_f: String!
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        if segue.identifier == "GoToOrb" {
            let dvc = segue.destination as! VideoViewController
            
            dvc.id  = num
            
            dvc.text = text_f
        }
    }
    
    //переход к камере
    @IBAction func GoToOrb(_ sender: Any) {
        performSegue(withIdentifier: "GoToOrb", sender: self)
    }
    
    
    //функция чтобы при нажатии ввод клава убиралась
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        self.view.endEditing(true)
        text_f = textField.text
        return false
    }
    
    //переменная для передачи картинки из первого окна
    var newImage: UIImage!
    
    //окно картинки с кейпоинтсами
    @IBOutlet weak var ImageWithKeypoints: UIImageView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        ImageWithKeypoints.image = newImage
        print(newImage.size)
        
        //вызываем функцию поиска пездатых кейпоинтсов и получаем массив из них
        let pointer = MatchingAlgorithmsBridge().findBest(newImage)

        
        //делаем массив нормальный бля
        var array = Array(UnsafeBufferPointer(start: pointer, count: 20))
        
        let heightInPoints = newImage.size.height

        let widthInPoints = newImage.size.width
        //get the frame of the square
        
        //edit square
        print(ImageWithKeypoints.frame.size.width, ImageWithKeypoints.frame.size.height, "size of imageView")
        print(ImageWithKeypoints.frame.origin.x, ImageWithKeypoints.frame.origin.y, "origin")
        
        print(array[0], array[1], "before")
        for i in 0...19 {
            if (i % 2 == 0) {
                array[i] = Float(CGFloat(array[i]) * ImageWithKeypoints.frame.size.width / widthInPoints)
            } else {
                array[i] = Float(CGFloat(array[i]) * ImageWithKeypoints.frame.size.height / heightInPoints + 75)
            }
        }
        print(array[0], array[1], "after")
        //проходим по нему и создаем кнопки с нужными координатами
        for i in 0...9 {
            //создаем кнопку с координатами
            makeButton(id: i, x: CGFloat(array[2 * i]), y: CGFloat(array[2 * i + 1]))
        }


        // Do any additional setup after loading the view.
    }
    
    //функция для создания кнопок
    
    func makeButton(id: Int, x: CGFloat, y: CGFloat) {
        let button = UIButton()
        button.frame = CGRect(x: x, y: y, width: 20, height: 20)
       
        button.backgroundColor = .green
        button.setTitle("\(id)", for: .normal)
        button.addTarget(self, action: #selector(buttonAction), for: .touchUpInside)
        button.tag = id
        self.view.addSubview(button);
        
    }
    
    //действие для кнопки
    @objc func buttonAction(sender: UIButton!) {
        
        //создаем текстововое окно на месте этой кнопки с таким же размером
        let label = UITextField(frame: sender.frame)
        label.backgroundColor = .red
        label.delegate = self
        self.view.addSubview(label)
        num = sender.tag
        
        
        //стираем кнопку
        sender.removeFromSuperview()
        
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */

}
