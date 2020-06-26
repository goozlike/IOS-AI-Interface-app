//
//  TableViewCell.swift
//  project_app
//
//  Created by Никита Борисов on 18.06.2020.
//  Copyright © 2020 Arina Goloubitskaya. All rights reserved.
//

import Foundation
import UIKit

class TableViewCell: UITableViewCell {
    
//      override init(style: UITableViewCell.CellStyle, reuseIdentifier: String?) {
//            super.init(style: style, reuseIdentifier: reuseIdentifier)
//            addSubview(fileName)
//        }
//
//      required init?(coder: NSCoder) {
//        fatalError("init(coder:) has not been implemented")
//      }
//
//        func configureTitleLabel() {
//            fileName.numberOfLines = 1
//            fileName.adjustsFontSizeToFitWidth = true
//        }
//
//        func setTitleLabelConstraints() {
//            fileName.translatesAutoresizingMaskIntoConstraints = false
//            fileName.centerYAnchor.constraint(equalTo: centerYAnchor).isActive = true
//            fileName.heightAnchor.constraint(equalToConstant: 80).isActive = true
//    }
    
    
    override func awakeFromNib() {
        super.awakeFromNib()
    }

    override func setSelected(_ selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)
    }

}
