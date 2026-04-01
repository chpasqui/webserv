/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdougoud <hdougoud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 10:28:06 by hdougoud          #+#    #+#             */
/*   Updated: 2025/12/16 16:35:05 by hdougoud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COLOR_HPP
# define COLOR_HPP

# define C_GREEN	"\033[38;5;47m"
# define C_BLUE		"\033[38;5;117m"
# define C_RED		"\033[38;5;196m"
# define C_YELLOW	"\033[38;5;220m"
# define C_PURPLE	"\033[38;5;105m"
# define C_PINK		"\033[38;5;206m"
# define C_CORAL	"\033[38;5;210m"
# define C_MAGENTA	"\033[38;5;201m"
# define C_WHITE	"\033[38;5;15m"
# define C_SPRING   "\033[38;5;42m"

//C++ Colors
# define C_CSTR		"\033[38;5;10m"		//Constructor color
# define C_ASNM		"\033[38;5;117m"	//Assignment operator
# define C_DSTR		"\033[38;5;202m"	//Destructor color

//Format
# define F_BOLD			"\033[1m"
# define F_UNDERLINED	"\033[4m"

# define RESET			"\033[0m"

#endif
