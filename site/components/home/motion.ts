import gsap from 'gsap'
import { ScrollTrigger } from 'gsap/ScrollTrigger'
import { TextPlugin } from 'gsap/TextPlugin'
import { useGSAP } from '@gsap/react'

gsap.registerPlugin(useGSAP, ScrollTrigger, TextPlugin)

export const moving = '(prefers-reduced-motion: no-preference)'

export { gsap, useGSAP }
